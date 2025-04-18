// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/gpio/consumer.h>
#include <linux/kernel.h>

#define DRIVER_NAME "foo-spi"

MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Example SPI driver");
MODULE_LICENSE("GPL");

static struct spi_device *spi_dev = NULL;
static struct gpio_desc *rst_gpio;

static int foo_spi_write(struct spi_device *spi, const u8 *data, size_t len)
{
	char debug_msg[256];
	sprintf(debug_msg, "LEN: 0x%lX\n", len);
	dev_info(&spi->dev, debug_msg);
	struct spi_transfer t = 
	{
		.tx_buf = data,
		.len = len,
	};
	struct spi_message m;
	spi_message_init(&m);
	spi_message_add_tail(&t, &m);

	return spi_sync(spi, &m);
}

static int foo_spi_read(struct spi_device *spi, const u8 *buf, size_t len)
{
	u8 tx[len];
	memset(tx, 0x00, len);

	struct spi_transfer t = 
	{
		.tx_buf = tx,
		.rx_buf = buf,
		.len = len,
	};

	struct spi_message m;
	spi_message_init(&m);
	spi_message_add_tail(&t, &m);

	return spi_sync(spi, &m);
}


static int foo_spi_probe(struct spi_device *spi)
{
	dev_info(&spi->dev, "foo-spi device probed!\n");
	spi_dev = spi;
	return 0;
}

static void foo_spi_remove(struct spi_device *spi)
{
	dev_info(&spi->dev, "foo-spi device removed.\n");
	spi_dev = NULL;
}

static const struct of_device_id foo_spi_of_match[] = {
	{ .compatible = "example,foo-spi" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, foo_spi_of_match);

static struct spi_driver foo_spi_driver = {
	.driver = {
		.name = DRIVER_NAME,
        	.of_match_table = foo_spi_of_match,
    	},
    	.probe = foo_spi_probe,
    	.remove = foo_spi_remove,
};
//module_spi_driver(foo_spi_driver);

struct foo_data
{
	struct gpio_desc *reset_gpio;
};

static int __init foo_spi_driver_mod_init(void)
{
	if(spi_register_driver(&foo_spi_driver))
	{
		pr_err("FAILED TO REGISTER SPI DRIVER!\n");
		return -1;
	}

	struct foo_data *data;
	data = devm_kzalloc(&spi_dev->dev, sizeof(*data), GFP_KERNEL);
	if(!data) return -ENOMEM;

	pr_info("INIT SPI DRIVER\n");

	data->reset_gpio = devm_gpiod_get(&spi_dev->dev, "reset", GPIOD_OUT_LOW);
	if(IS_ERR(data->reset_gpio))
	{
		pr_err("FAILED TO GET RST GPIO!\n");
		return PTR_ERR(data->reset_gpio);
	}

	gpiod_set_value(data->reset_gpio, 1);
	spi_set_drvdata(spi_dev, data);
	return 0;
}

static void __exit foo_spi_driver_mod_exit(void)
{
	spi_unregister_driver(&foo_spi_driver);
	gpiod_put(rst_gpio);
	pr_info("EXIT SPI DRIVER\n");
}

module_init(foo_spi_driver_mod_init);
module_exit(foo_spi_driver_mod_exit);

























