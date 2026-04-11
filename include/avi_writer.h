// FEMTOCAM — avi_writer.h
// Copyright (c) 2025 Gorogepapa (meph6346@gmail.com) — MIT License
#pragma once
#include <Arduino.h>
#include "SD_MMC.h"

#define AVI_MAX_FRAMES 50000

class AviWriter {
public:
    bool begin(const char* path, uint16_t w, uint16_t h, uint8_t fps=1) {
        _f=SD_MMC.open(path,FILE_WRITE); if(!_f)return false;
        _w=w;_h=h;_fps=fps;_n=0;_dataSize=0;
        _idx=(Idx*)ps_malloc(sizeof(Idx)*AVI_MAX_FRAMES);
        if(!_idx){_f.close();return false;}
        writeHeader(); return true;
    }
    bool addFrame(const uint8_t*jpg,size_t len){
        if(!_f||_n>=AVI_MAX_FRAMES)return false;
        size_t pad=len&1;
        _idx[_n]={_dataSize+4,(uint32_t)len};
        w4("00dc");w32(len);_f.write(jpg,len);
        if(pad)_f.write((uint8_t)0);
        _dataSize+=8+len+pad;_n++; return true;
    }
    void flush(){
        if(!_f||!_n)return;
        uint32_t pos=_f.position(); writeIndex();
        uint32_t end=_f.position(); patch(end); _f.seek(pos);
    }
    bool finalize(){
        if(!_f)return false;
        writeIndex(); uint32_t end=_f.position(); patch(end);
        _f.close(); if(_idx){free(_idx);_idx=NULL;} return true;
    }
    void abort(){if(_f)_f.close();if(_idx){free(_idx);_idx=NULL;}}
    uint32_t count()const{return _n;}
private:
    struct Idx{uint32_t off,sz;};
    File _f; Idx*_idx=NULL; uint16_t _w,_h; uint8_t _fps;
    uint32_t _n,_dataSize,_avihF,_moviS;
    void w4(const char*c){_f.write((const uint8_t*)c,4);}
    void w16(uint16_t v){_f.write(v&0xFF);_f.write((v>>8)&0xFF);}
    void w32(uint32_t v){uint8_t b[4]={(uint8_t)v,(uint8_t)(v>>8),(uint8_t)(v>>16),(uint8_t)(v>>24)};_f.write(b,4);}
    void writeIndex(){w4("idx1");w32(_n*16);for(uint32_t i=0;i<_n;i++){w4("00dc");w32(0x10);w32(_idx[i].off);w32(_idx[i].sz);}}
    void patch(uint32_t end){_f.seek(4);w32(end-8);_f.seek(_avihF);w32(_n);_f.seek(_moviS);w32(_dataSize+4);}
    void writeHeader(){
        uint32_t us=_fps>0?1000000/_fps:1000000;
        w4("RIFF");w32(0);w4("AVI ");
        w4("LIST");uint32_t hs=_f.position();w32(0);w4("hdrl");
        w4("avih");w32(56);w32(us);w32(0);w32(0);w32(0);
        _avihF=_f.position();w32(0);w32(0);w32(1);w32(0);w32(_w);w32(_h);
        w32(0);w32(0);w32(0);w32(0);
        w4("LIST");w32(116);w4("strl");
        w4("strh");w32(56);w4("vids");w4("MJPG");w32(0);w16(0);w16(0);w32(0);
        w32(1);w32(_fps>0?_fps:1);w32(0);w32(0);w32(0);w32(0);w32(0);w16(0);w16(0);w16(_w);w16(_h);
        w4("strf");w32(40);w32(40);w32(_w);w32(_h);w16(1);w16(24);
        w4("MJPG");w32(_w*_h*3);w32(0);w32(0);w32(0);w32(0);
        uint32_t he=_f.position();_f.seek(hs);w32(he-hs-4);_f.seek(he);
        w4("LIST");_moviS=_f.position();w32(0);w4("movi");
    }
};
