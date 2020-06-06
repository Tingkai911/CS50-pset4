#include "helpers.h"
#include <stdbool.h>
#include <math.h>

typedef struct
{
    float Red;
    float Green;
    float Blue;
}
RGBCALC;

// Convert image to grayscale DONE
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j++)
        {
            int average = round( (image[i][j].rgbtBlue + image[i][j].rgbtGreen + image[i][j].rgbtRed) / 3.0 );
            image[i][j].rgbtBlue = average;
            image[i][j].rgbtGreen = average;
            image[i][j].rgbtRed = average;
        }
    }

    return;
}

// Reflect image horizontally DONE
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<round(width/2) ; j++)
        {
            RGBTRIPLE temp = image[i][j];
            image[i][j] = image[i][width - 1 - j];
            image[i][width - 1 - j] = temp;
        }
    }

    return;
}

bool is_valid_pixel (int i_new, int j_new, int height, int width)
{
    if (i_new >= 0 && j_new >=0 && i_new < height && j_new < width)
    {
        return true;
    }

    return false;
}

RGBTRIPLE get_blured_pixel(int i, int j, int height, int width, RGBTRIPLE image[height][width])
{
    float redValue, greenValue, blueValue;
    int numOfValidPixel = 0;
    redValue = greenValue = blueValue = 0;

    for (int di = -1; di <= 1; di++)
    {
        int i_new = i + di;
        for (int dj = -1; dj <= 1; dj ++)
        {
            int j_new = j + dj;
            if(is_valid_pixel(i_new, j_new, height, width) == true)
            {
                redValue = redValue + image[i_new][j_new].rgbtRed;
                greenValue = greenValue + image[i_new][j_new].rgbtGreen;
                blueValue = blueValue + image[i_new][j_new].rgbtBlue;
                numOfValidPixel++;
            }
        }
    }

    RGBTRIPLE blured_pixel;
    blured_pixel.rgbtRed =  round(redValue / numOfValidPixel);
    blured_pixel.rgbtGreen = round(greenValue / numOfValidPixel);
    blured_pixel.rgbtBlue = round(blueValue / numOfValidPixel);

    return blured_pixel;
}

// Blur image DONE
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE new_image[height][width];

    for (int i=0; i<height; i++)
    {
        for(int j=0; j<width; j++)
        {
            new_image[i][j] = get_blured_pixel(i, j, height, width, image);
        }
    }

    for (int i=0; i<height; i++)
    {
        for(int j=0; j<width; j++)
        {
            image[i][j] = new_image[i][j];
        }
    }

    return;
}

RGBCALC calc_Gx(int i, int j, int height, int width, RGBTRIPLE image[height][width])
{
    RGBCALC Gx;

    int matrix_Gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    float redValue, greenValue, blueValue;
    redValue = greenValue = blueValue = 0;

    for (int di = -1; di <= 1; di++)
    {
        int i_new = i + di;
        for (int dj = -1; dj <= 1; dj++)
        {
            int j_new = j + dj;
            if(is_valid_pixel(i_new, j_new, height, width) == true)
            {
                redValue += matrix_Gx[di + 1][dj + 1] * image[i_new][j_new].rgbtRed;
                blueValue += matrix_Gx[di + 1][dj + 1] * image[i_new][j_new].rgbtBlue;
                greenValue += matrix_Gx[di + 1][dj + 1] * image[i_new][j_new].rgbtGreen;
            }
        }
    }

    Gx.Red = redValue;
    Gx.Blue = blueValue;
    Gx.Green = greenValue;

    return Gx;
}

RGBCALC calc_Gy(int i, int j, int height, int width, RGBTRIPLE image[height][width])
{
    RGBCALC Gy;

    int matrix_Gy[3][3] = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}
    };

    float redValue, greenValue, blueValue;
    redValue = greenValue = blueValue = 0;

    for (int di = -1; di <= 1; di++)
    {
        int i_new = i + di;
        for (int dj = -1; dj <= 1; dj++)
        {
            int j_new = j + dj;
            if(is_valid_pixel(i_new, j_new, height, width) == true)
            {
                redValue += matrix_Gy[di + 1][dj + 1] * image[i_new][j_new].rgbtRed;
                blueValue += matrix_Gy[di + 1][dj + 1] * image[i_new][j_new].rgbtBlue;
                greenValue += matrix_Gy[di + 1][dj + 1] * image[i_new][j_new].rgbtGreen;
            }
        }
    }

    Gy.Red = redValue;
    Gy.Blue = blueValue;
    Gy.Green = greenValue;

    return Gy;
}

RGBTRIPLE get_edge_pixel(int i, int j, int height, int width, RGBTRIPLE image[height][width])
{
    RGBCALC Gx = calc_Gx(i, j, height, width, image);
    RGBCALC Gy = calc_Gy(i, j, height, width, image);

    RGBTRIPLE new_channel_value;

    float redValue, greenValue, blueValue;

    redValue = round(sqrt(Gx.Red*Gx.Red + Gy.Red*Gy.Red));
    blueValue = round(sqrt(Gx.Blue*Gx.Blue + Gy.Blue*Gy.Blue));
    greenValue = round(sqrt(Gx.Green*Gx.Green + Gy.Green*Gy.Green));

    if (redValue > 255 )
        new_channel_value.rgbtRed = 255;
    else
        new_channel_value.rgbtRed = redValue;

    if (blueValue > 255 )
        new_channel_value.rgbtBlue = 255;
    else
        new_channel_value.rgbtBlue = blueValue;

    if (greenValue > 255 )
        new_channel_value.rgbtGreen = 255;
    else
        new_channel_value.rgbtGreen = greenValue;

    return new_channel_value;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE edge_image[height][width];

    for (int i = 0; i<height; i++)
    {
        for (int j = 0; j<width; j++)
        {
            edge_image[i][j] = get_edge_pixel(i, j, height, width, image);
        }
    }

    for (int i=0; i<height; i++)
    {
        for(int j=0; j<width; j++)
        {
            image[i][j] = edge_image[i][j];
        }
    }

    return;
}
