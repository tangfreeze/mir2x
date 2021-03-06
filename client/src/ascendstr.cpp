/*
 * =====================================================================================
 *
 *       Filename: ascendstr.cpp
 *        Created: 07/20/2017 00:34:13
 *    Description:
 *
 *        Version: 1.0
 *       Revision: none
 *       Compiler: gcc
 *
 *         Author: ANHONG
 *          Email: anhonghe@gmail.com
 *
 * =====================================================================================
 */
#include <cstdint>
#include <cinttypes>
#include "log.hpp"
#include "pngtexdb.hpp"
#include "ascendstr.hpp"
#include "sdldevice.hpp"

extern Log *g_log;
extern PNGTexDB *g_progUseDB;
extern SDLDevice *g_sdlDevice;

AscendStr::AscendStr(int nType, int nValue, int nX, int nY)
    : m_type(nType)
    , m_value(nValue)
    , m_x(nX)
    , m_y(nY)
    , m_tick(0.0)
{
    switch(m_type){
        case ASCENDSTR_MISS:
            {
                m_value = 0;
                break;
            }
        case ASCENDSTR_NUM0:
        case ASCENDSTR_NUM1:
        case ASCENDSTR_NUM2:
            {
                break;
            }
        default:
            {
                g_log->addLog(LOGTYPE_FATAL, "Invalid AscendStr type: %d", Type());
                break;
            }
    }

    // TODO
    // should I here put checks for (nX, nY) in current map?
}

void AscendStr::Update(double fUpdate)
{
    m_tick += fUpdate;
}

void AscendStr::Draw(int nViewX, int nViewY)
{
    if(Ratio() < 1.0){

        auto nCurrX = X();
        auto nCurrY = Y();
        auto nCurrA = (Uint8)(std::lround(255 * (1.0 - Ratio())));

        switch(Type()){
            case ASCENDSTR_MISS:
                {
                    if(auto pTexture = g_progUseDB->Retrieve(0X03000030)){
                        SDL_SetTextureAlphaMod(pTexture, nCurrA);
                        g_sdlDevice->drawTexture(pTexture, nCurrX - nViewX, nCurrY - nViewY);
                    }
                    break;
                }
            case ASCENDSTR_NUM0:
            case ASCENDSTR_NUM1:
            case ASCENDSTR_NUM2:
                {
                    if(Value()){
                        uint32_t nPreKey = 0X03000000 | ((Type() - ASCENDSTR_NUM0) << 4);
                        if(auto pTexture = g_progUseDB->Retrieve(nPreKey | ((Value() < 0) ? 0X0A : 0X0B))){
                            SDL_SetTextureAlphaMod(pTexture, nCurrA);
                            g_sdlDevice->drawTexture(pTexture, nCurrX - nViewX, nCurrY - nViewY + ((Value() < 0) ? 4 : 1));

                            int nTextureW;
                            SDL_QueryTexture(pTexture, nullptr, nullptr, &nTextureW, nullptr);
                            nCurrX += nTextureW;
                        }

                        auto szNumStr = std::to_string(std::labs(Value()));
                        for(auto chNum: szNumStr){
                            if(auto pTexture = g_progUseDB->Retrieve(nPreKey | (chNum - '0'))){
                                SDL_SetTextureAlphaMod(pTexture, nCurrA);
                                g_sdlDevice->drawTexture(pTexture, nCurrX - nViewX, nCurrY - nViewY);

                                int nTextureW;
                                SDL_QueryTexture(pTexture, nullptr, nullptr, &nTextureW, nullptr);
                                nCurrX += nTextureW;
                            }
                        }
                    }
                    break;
                }
            default:
                {
                    break;
                }
        }
    }
}
