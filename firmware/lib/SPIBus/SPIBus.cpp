#include "SPIBus.hpp"

SPIBus::SPIBus(uint8_t packet[], mutex_t& p_mutex): 
    packet_(packet), p_mutex_(p_mutex) {}

SPIError SPIBus::init() {
    // initializes spi and sets to 1 MHz
    spi_init(SPI_MASTER, 1000*1000);
    spi_init(SPI_SLAVE,  1000*1000);
    spi_set_slave(SPI_SLAVE, true);
    
    // sets SPI mode 3: clock idle is high, data is shifted on falling/leading edge, and data is sampled on rising/trailing edge
    spi_set_format(SPI_MASTER, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
    spi_set_format(SPI_SLAVE,  8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);

    // sets other pins to SPI
    gpio_set_function(PIN_MISO_MASTER, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS_MASTER,   GPIO_FUNC_SIO); // SIO to manually control chip select
    gpio_set_function(PIN_SCK_MASTER,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI_MASTER, GPIO_FUNC_SPI);

    gpio_put(PIN_CS_MASTER, 1); // Chip select is active low
    gpio_set_dir(PIN_CS_MASTER, GPIO_OUT);

    gpio_set_function(PIN_MISO_SLAVE, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS_SLAVE,   GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK_SLAVE,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI_SLAVE, GPIO_FUNC_SPI);

    // intializes the interrupt for when the previous device sends data
    bus_ = this;
    gpio_set_irq_enabled_with_callback(PIN_CS_SLAVE, GPIO_IRQ_EDGE_FALL, true, &handle_cs_fall);

    return SPIError::NO_ERROR;
}

SPIError SPIBus::init_dma() {
    dma_rx_ = dma_claim_unused_channel(false);
    dma_tx_ = dma_claim_unused_channel(false); 

    if ((dma_rx_ == -1) || (dma_tx_ == -1)) return SPIError::NO_DMA_CHANNEL;

    config_rx_ = dma_channel_get_default_config(dma_rx_);
    config_tx_ = dma_channel_get_default_config(dma_tx_);

    // always transfers 8 bits at a time
    channel_config_set_transfer_data_size(&config_rx_, DMA_SIZE_8);
    channel_config_set_transfer_data_size(&config_tx_, DMA_SIZE_8);

    // increments shouldn't be needed for FIFO, only memory
    channel_config_set_read_increment(&config_rx_,  false);
    channel_config_set_write_increment(&config_rx_,  true); 
    channel_config_set_read_increment(&config_tx_,   true);
    channel_config_set_write_increment(&config_tx_, false);

    channel_config_set_dreq(&config_rx_, spi_get_dreq(SPI_SLAVE, false));
    channel_config_set_dreq(&config_tx_, spi_get_dreq(SPI_MASTER, true));

    dma_channel_configure(dma_rx_, &config_rx_, buffer_,  &spi_get_hw(SPI_SLAVE)->dr, PACKET_LENGTH, false);
    dma_channel_configure(dma_tx_, &config_tx_, &spi_get_hw(SPI_MASTER)->dr, packet_, PACKET_LENGTH, false);

    dma_channel_set_irq0_enabled(dma_rx_, true);
    dma_channel_set_irq0_enabled(dma_tx_, true);

    irq_set_exclusive_handler(DMA_IRQ_0, &handle_dma);
    irq_set_enabled(DMA_IRQ_0, true);

    return SPIError::NO_ERROR;
}

SPIError SPIBus::reset_dma_rx(bool init) {
    dma_channel_abort(dma_rx_);
    dma_hw->ints0 = 1u << dma_rx_;

    while (spi_is_readable(SPI_SLAVE)) {
        (void)spi_get_hw(SPI_SLAVE)->dr;
    }

    if (init) dma_channel_configure(dma_rx_, &config_rx_, buffer_,  &spi_get_hw(SPI_SLAVE)->dr, PACKET_LENGTH, false);
    
    return SPIError::NO_ERROR;
}

SPIError SPIBus::reset_dma_tx(bool init) {
    irq_set_enabled(DMA_IRQ_0, false);
    if (alarm_ > 0) { 
        cancel_alarm(alarm_); 
        alarm_ = 0; 
    }

    dma_channel_abort(dma_tx_);
    dma_hw->ints0 = 1u << dma_tx_;

    // not sure how else to make sure the FIFO is empty
    spi_deinit(SPI_MASTER);
    spi_init(SPI_MASTER, 1000*1000);
    spi_set_format(SPI_MASTER, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);

    gpio_put(PIN_CS_MASTER, 1);
    
    if (init) dma_channel_configure(dma_tx_, &config_tx_, &spi_get_hw(SPI_MASTER)->dr, packet_, PACKET_LENGTH, false);
    irq_set_enabled(DMA_IRQ_0, true);

    return SPIError::NO_ERROR;
}

SPIError SPIBus::read_blocking() {
    spi_read_blocking(SPI_SLAVE, 0, buffer_, PACKET_LENGTH);
    new_data_ = true;
    return SPIError::NO_ERROR;
}

SPIError SPIBus::update_packet() {
    SPIError error = SPIError::NO_ERROR;

    // makes sure previous device bits in checksum are high
    if ((buffer_[PACKET_LENGTH - 1] & 0b00111111) != 0b00111111) {
        error = SPIError::BAD_CHECKSUM;
        if (checksum_required_) return error;
    } 

    // updates checksum and writes the buffer to the main data store
    buffer_[PACKET_LENGTH - 1] |= (1u << 6);
    mutex_enter_blocking(&p_mutex_);

    for (int i = 0; i < PACKET_LENGTH; i++) {
        packet_[i] = buffer_[i];
    }

    mutex_exit(&p_mutex_);

    return error;
}

SPIError SPIBus::write_blocking() {
    mutex_enter_blocking(&p_mutex_);

    gpio_put(PIN_CS_MASTER, 0);
    spi_write_blocking(SPI_MASTER, packet_, PACKET_LENGTH);
    gpio_put(PIN_CS_MASTER, 1);

    mutex_exit(&p_mutex_);

    return SPIError::NO_ERROR;
}

SPIError SPIBus::read_nonblocking() {
    if (!dma_channel_is_busy(dma_rx_)) dma_channel_start(dma_rx_);

    return SPIError::NO_ERROR;
}

SPIError SPIBus::write_nonblocking() {
    // mutex is unlocked in handle_dma() when the DMA transfer completes
    // chip select is put high in handle_alarm() when the SPI transfer completes
    mutex_enter_blocking(&p_mutex_); 

    gpio_put(PIN_CS_MASTER, 0);
    dma_channel_start(dma_tx_);
    
    return SPIError::NO_ERROR;
}

void SPIBus::handle_cs_fall(uint gpio, uint32_t events) {
    (bus_->*(bus_->callback_read_))();
}

void SPIBus::handle_dma() {
    uint32_t interrupts = dma_hw->ints0;

    if (interrupts & (1u << bus_->dma_rx_)) {
        dma_channel_acknowledge_irq0(bus_->dma_rx_);
        dma_channel_configure(bus_->dma_rx_, &(bus_->config_rx_), bus_->buffer_, &spi_get_hw(SPI_SLAVE)->dr, PACKET_LENGTH, bus_->auto_read_nonblocking_);

        bus_->new_data_ = true;
    }

    if (interrupts & (1u << bus_->dma_tx_)) {
        dma_channel_acknowledge_irq0(bus_->dma_tx_);
        
        // waits for 80 us before pulling chip select high to give time for the actual transfer to finish.
        bus_->alarm_ = add_alarm_in_us(80, &handle_alarm, this, false);
        dma_channel_configure(bus_->dma_tx_, &(bus_->config_tx_), &spi_get_hw(SPI_MASTER)->dr, bus_->packet_, PACKET_LENGTH, false);

        mutex_exit(&(bus_->p_mutex_));
    }
}

int64_t SPIBus::handle_alarm(alarm_id_t id, void *user_data) {
    if (spi_is_busy(SPI_MASTER)) return 5;

    gpio_put(PIN_CS_MASTER, 1);
    
    return 0;
}

void SPIBus::set_auto_read_nonblocking(bool automatic) {
    auto_read_nonblocking_ = automatic;
}

void SPIBus::set_checksum_required(bool required) {
    checksum_required_ = required;
}

void SPIBus::set_callback_read(bool blocking) {
    callback_read_ = blocking ? &SPIBus::read_blocking : &SPIBus::read_nonblocking;
}