/*
 * =====================================================================================
 *
 *       Filename: dispatcher.hpp
 *        Created: 01/26/2018 15:36:14
 *    Description:
 *                 anonymous message sender
 *                 actor who receives the message will uid zero
 *
 *        Version: 1.0
 *       Revision: none
 *       Compiler: gcc
 *
 *         Author: ANHONG
 *          Email: anhonghe@gmail.com
 *   Organization: USTC
 *
 * =====================================================================================
 */

#pragma once
#include <cstdint>
#include "actormsgbuf.hpp"

class Dispatcher
{
    public:
        Dispatcher() = default;

    public:
        virtual ~Dispatcher() = default;

    public:
        bool forward(uint64_t, const ActorMsgBuf &, uint32_t = 0);
};
