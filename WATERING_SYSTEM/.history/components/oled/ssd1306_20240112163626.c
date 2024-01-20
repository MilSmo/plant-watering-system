#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "ssd1306.h"
#include "font8x8_basic.h"

#define TAG "SSD1306"

#define PACK8 __attribute__((aligned(__alignof__(uint8_t)), packed))

typedef union out_column_t
{
	uint32_t u32;
	uint8_t u8[4];
} PACK8 out_column_t;

void ssd1306_init(SSD1306_t *dev, int width, int height)
{
	i2c_init(dev, width, height);

	for (int i = 0; i < dev->_pages; i++)
	{
		memset(dev->_page[i]._segs, 0, 128);
	}
}

int ssd1306_get_width(SSD1306_t *dev)
{
	return dev->_width;
}

int ssd1306_get_height(SSD1306_t *dev)
{
	return dev->_height;
}

int ssd1306_get_pages(SSD1306_t *dev)
{
	return dev->_pages;
}


void ssd1306_display_image(SSD1306_t *dev, int page, int seg, uint8_t *images, int width)
{

	i2c_display_image(dev, page, seg, images, width);

	memcpy(&dev->_page[page]._segs[seg], images, width);
}

void ssd1306_display_text(SSD1306_t *dev, int page, char *text, int text_len, bool invert)
{
	if (page >= dev->_pages)
		return;
	int _text_len = text_len;
	if (_text_len > 16)
		_text_len = 16;

	uint8_t seg = 0;
	uint8_t image[8];
	for (uint8_t i = 0; i < _text_len; i++)
	{
		memcpy(image, font8x8_basic_tr[(uint8_t)text[i]], 8);
		if (invert)
			ssd1306_invert(image, 8);
		if (dev->_flip)
			ssd1306_flip(image, 8);
		ssd1306_display_image(dev, page, seg, image, 8);
		seg = seg + 8;
	}
}

void ssd1306_clear_screen(SSD1306_t *dev, bool invert)
{
	char space[16];
	memset(space, 0x00, sizeof(space));
	for (int page = 0; page < dev->_pages; page++)
	{
		ssd1306_display_text(dev, page, space, sizeof(space), invert);
	}
}

void ssd1306_clear_line(SSD1306_t *dev, int page, bool invert)
{
	char space[16];
	memset(space, 0x00, sizeof(space));
	ssd1306_display_text(dev, page, space, sizeof(space), invert);
}

void ssd1306_contrast(SSD1306_t *dev, int contrast)
{

	i2c_contrast(dev, contrast);
}

void ssd1306_invert(uint8_t *buf, size_t blen)
{
	uint8_t wk;
	for (int i = 0; i < blen; i++)
	{
		wk = buf[i];
		buf[i] = ~wk;
	}
}

// Flip upside down
void ssd1306_flip(uint8_t *buf, size_t blen)
{
	for (int i = 0; i < blen; i++)
	{
		buf[i] = ssd1306_rotate_byte(buf[i]);
	}
}

// Rotate 8-bit data
// 0x12-->0x48
uint8_t ssd1306_rotate_byte(uint8_t ch1)
{
	uint8_t ch2 = 0;
	for (int j = 0; j < 8; j++)
	{
		ch2 = (ch2 << 1) + (ch1 & 0x01);
		ch1 = ch1 >> 1;
	}
	return ch2;
}






// void ssd1306_show_buffer(SSD1306_t *dev)
// {

// 	for (int page = 0; page < dev->_pages; page++)
// 	{
// 		i2c_display_image(dev, page, 0, dev->_page[page]._segs, dev->_width);
// 	}
// }

// void ssd1306_set_buffer(SSD1306_t *dev, uint8_t *buffer)
// {
// 	int index = 0;
// 	for (int page = 0; page < dev->_pages; page++)
// 	{
// 		memcpy(&dev->_page[page]._segs, &buffer[index], 128);
// 		index = index + 128;
// 	}
// }

// void ssd1306_get_buffer(SSD1306_t *dev, uint8_t *buffer)
// {
// 	int index = 0;
// 	for (int page = 0; page < dev->_pages; page++)
// 	{
// 		memcpy(&buffer[index], &dev->_page[page]._segs, 128);
// 		index = index + 128;
// 	}
// }
