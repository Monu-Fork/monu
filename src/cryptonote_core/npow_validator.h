/*
--------------------------------------------------
    James William Fletcher (james@voxdsp.com)
        July 2021
--------------------------------------------------
    white paper;
    https://raw.githubusercontent.com/Monu-Fork/NPOW/main/npow_whitepaper.pdf

    For use in Monero fork Monu
    https://github.com/monu-fork
*/

#ifndef NPOW_VALIDATOR
#define NPOW_VALIDATOR

// functions
float NPOW_toEmbed(signed char c);
char  NPOW_fromEmbed(const float f);
int   NPOW_check(const char* hexstring_input, const char* hexstring_target, const float* weights);

// float32 to int8 quantisation and vice-versa
// #define SCALE_FACTOR 28  // Recommended factor: 32 - 128
//                          // 8   = ~0.13 to 16.0
//                          // 16  = ~0.07 to 8.0
//                          // 32  = ~0.04 to 4.0
//                          // 64  = ~0.02 to 2.0
//                          // 128 = ~0.01 to 1.0
// uint8_t NPOW_packFloat(const float f)
// {
//     float r = (f * SCALE_FACTOR);
//     if(r > 0){r += 0.5;}
//     else if(r < 0){r -= 0.5;}
//     if(r >= 127){return 127;}
//     else if(r <= -128){return -128;}
//     return (uint8_t)r;
// }
// float NPOW_unpackFloat(const uint8_t f)
// {
//     return (float)(f / SCALE_FACTOR);
// }

// code
float NPOW_toEmbed(char c)
{
    // lower case
    if(c >= 97){c -= 32;}
    // returns normalised embedding -0.8828125 to +0.875 range
         if(c == '0'){return -0.8828125;}
    else if(c == '1'){return -0.765625;}
    else if(c == '2'){return -0.6484375;}
    else if(c == '3'){return -0.53125;}
    else if(c == '4'){return -0.4140625;}
    else if(c == '5'){return -0.296875;}
    else if(c == '6'){return -0.1796875;}
    else if(c == '7'){return -0.0625;}
    else if(c == '8'){return  0.0546875;}
    else if(c == '9'){return  0.171875;}
    else if(c == 'a'){return  0.2890625;}
    else if(c == 'b'){return  0.40625;}
    else if(c == 'c'){return  0.5234375;}
    else if(c == 'd'){return  0.640625;}
    else if(c == 'e'){return  0.7578125;}
    else if(c == 'f'){return  0.875;}
}

char NPOW_fromEmbed(const float f)
{
    // returns characters from a -1 to +1 range
         if(f > -1 && f < -0.82421875)          {return '0';}
    else if(f >= -0.82421875 && f < -0.70703125){return '1';}
    else if(f >= -0.70703125 && f < -0.58984375){return '2';}
    else if(f >= -0.58984375 && f < -0.47265625){return '3';}
    else if(f >= -0.47265625 && f < -0.35546875){return '4';}
    else if(f >= -0.35546875 && f < -0.23828125){return '5';}
    else if(f >= -0.23828125 && f < -0.12109375){return '6';}
    else if(f >= -0.12109375 && f < -0.00390625){return '7';}
    else if(f >= -0.00390625 && f <  0.11328125){return '8';}
    else if(f >=  0.11328125 && f <  0.23046875){return '9';}
    else if(f >=  0.23046875 && f <  0.34765625){return 'A';}
    else if(f >=  0.34765625 && f <  0.46484375){return 'B';}
    else if(f >=  0.46484375 && f <  0.58203125){return 'C';}
    else if(f >=  0.58203125 && f <  0.69921875){return 'D';}
    else if(f >=  0.69921875 && f <  0.81640625){return 'E';}
    else if(f >=  0.81640625 && f <  1)         {return 'F';}
}

int NPOW_check(const char* hexstring_input, const char* hexstring_target, const float* weights)
{
    // convert input to embedding
    float input[64];
    for(int i = 0; i < 64; i++)
      input[i] = NPOW_toEmbed(hexstring_input[i]);

    // forward prop layer one
    float o1[64] = {0};
    int u = 0, wc = 0;
    for(int i = 0; i < 4160; i++)
    {
        if(wc == 64)
        {
            o1[u] += weights[i]; // bias
            o1[u] = tanh(o1[u]);
            u++;
            wc = 0;
        }
        else
        {
            o1[u] += input[wc] * weights[i]; // weight
            wc++;
        }
    }

    // forward prop layer two
    float o2[64] = {0};
    u = 0, wc = 0;
    for(int i = 4160; i < 8320; i++)
    {
        if(wc == 64)
        {
            o2[u] += weights[i]; // bias
            u++;
            wc = 0;
        }
        else
        {
            o2[u] += o1[wc] * weights[i]; // weight
            wc++;
        }
    }

    // convert layer two output back to hex & fail at the first non-matching character
    for(int i = 0; i < 64; i++)
        if(NPOW_fromEmbed(o2[i]) != hexstring_target[i])
            return 0;

    // success
    return 1;
}

#endif
