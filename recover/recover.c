#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    FILE *img = NULL;

    if( f == NULL)
    {
        return 2;
    }

    // each JPEG file is stored back to back
    // read the file in 512bytes chunk
    BYTE buffer[512];
    bool found_first_jpg = false;
    int i = 0;
    char filename[10];

    while(fread(buffer, sizeof(BYTE), 512, f) != 0)
    {
        // first 3 bytes is always 0xff 0xd8 0xff
        // the 4th byte is always 0xe0 or 0xe1 .... 0xef
        if(buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            if(found_first_jpg == false)
            {
                found_first_jpg = true;

                // create a new jpg file
                sprintf(filename, "%03i.jpg", i);
                i++;
                img = fopen(filename, "w");
                if(img == NULL)
                {
                    return 3;
                }
                fwrite(buffer, sizeof(BYTE), 512, img);

            }
            else
            {
                // close the previous jpg file
                fclose(img);

                // create a new jpg file
                sprintf(filename, "%03i.jpg", i);
                i++;
                img = fopen(filename, "w");
                if(img == NULL)
                {
                    return 3;
                }
                fwrite(buffer, sizeof(BYTE), 512, img);

            }
        }
        else
        {
            // continue writing to the current file if we already found the jpg
            if(found_first_jpg == true){
                fwrite(buffer, sizeof(BYTE), 512, img);
            }
        }

    }

}
