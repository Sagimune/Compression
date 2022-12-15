#include "lzss.h"

LZSS::LZSS(BYTE* _stream, int len)
{
    memset(slideWindow, 0x00, sizeof(slideWindow));
    memset(buffer, 0x00, sizeof(buffer));
    swleft = swright = swcount = 0;
    bufferleft = bufferright = buffercount = 0;
    rescount = 0;

    //maxlen == MAXLZBUFFER
    int i;
    for(i = 0; i < len && i < MAXLZBUFFER; i ++ )
    {
        buffer[bufferright ++] = *(_stream);
        _stream ++;
    }
    bufferright %= MAXLZBUFFER;
    buffercount = i;

    stream = _stream;
    streamlen = len;
    reslen = len - i;
}

bool LZSS::read(int len)
{
    for(int i = 0; i < std::min(len, reslen); i ++)
    {
        if(buffercount < MAXLZBUFFER)
        {
            slideWindow[swright ++] = *(buffer + bufferleft);
            swcount ++;

            buffer[bufferright ++] = *(stream + i);
            buffercount ++;
        }
        else if(swcount < MAXLZSW)
        {
            slideWindow[swright ++] = *(buffer + bufferleft);
            swcount ++;

            buffer[bufferright ++] = *(stream);
            stream ++;
            bufferleft ++;
            bufferleft  %= MAXLZBUFFER;
            bufferright %= MAXLZBUFFER;
        }
        else
        {
            slideWindow[swright ++] = *(buffer + bufferleft);
            swleft ++;
            swleft  %= MAXLZSW;
            swright %= MAXLZSW;

            buffer[bufferright ++] = *(stream);
            stream ++;
            bufferleft ++;
            bufferleft  %= MAXLZBUFFER;
            bufferright %= MAXLZBUFFER;
        }
    }

    if(reslen < len)
    {
        int readreslen = len - reslen;
        reslen = 0;

        bufferleft = (bufferleft + readreslen) % MAXLZBUFFER;
        swleft = (swleft + readreslen) % MAXLZSW;
        buffercount -= readreslen;
        swcount -= readreslen;
    }
    else
    {
        reslen -= len;
    }

    return true;
}


bool LZSS::find()
{
    int length = -1;
    int distance = -1;
    for(int i = swleft; i != swright; i = (i + 1) % MAXLZSW)
    {
        if(slideWindow[i] == buffer[bufferleft])
        {
            for(int offset = 0; offset < std::min(buffercount, swcount); offset ++)
            {
                if(slideWindow[(i + offset) % MAXLZSW] == buffer[(bufferleft + offset) % MAXLZBUFFER])
                {
                    int current_len = offset + 1;
                    if(current_len >= MINLZLEN && current_len >= length)
                    {
                        length = current_len;
                        distance = (swright - i + MAXLZSW) % MAXLZSW;
                    }
                }
            }
        }
    }

    if(distance != -1 && length != -1)
    {
        ld2code(length, distance);
        return true;
    }

    return false;
}

void LZSS::ld2code(int length, int distance)
{
    //struct [length, result(4)]
    ldcode_res[0] = length;

    //length
    int lenidx = 0;
    while(length_code_list[lenidx] <= length) lenidx ++;
    lenidx --;
    //int extra_len = length_code_extra[lenidx];
    int lengap = length - length_code_list[lenidx];
    ldcode_res[1] = lenidx + 257;
    ldcode_res[2] = lengap;

    //distance
    int distidx = 0;
    while(distance_code_list[distidx] <= distance) distidx ++;
    distidx --;
    int distgap = distance - distance_code_list[distidx];
    ldcode_res[3] = distidx;
    ldcode_res[4] = distgap;

}

int* LZSS::dolzss()
{
    int count = 0;
    do
    {
        bool res = find();
        if(res)
        {
            for(int i = 1; i <= 4; i ++ ) result[++ rescount] = *(ldcode_res + i);
            read(ldcode_res[0]);
            count += ldcode_res[0];
        }
        else
        {
            result[++ rescount] = *(buffer + bufferleft);
            read(1);
            count += 1;
        }
    }while(count < streamlen);

    result[0] = rescount;
    return result;
}



LZSSDecode::LZSSDecode(int* stream, int len)
{
    for(int i = 0; i < len; i ++ )
    {
        if(stream[i] <= 255) result[rescount ++] = stream[i];
        else if(stream[i] == 256) continue;
        else
        {

        }
    }
}
