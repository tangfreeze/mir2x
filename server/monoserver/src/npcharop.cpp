/*
 * =====================================================================================
 *
 *       Filename: npcharop.cpp
 *        Created: 04/12/2020 16:27:40
 *    Description:
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

#include "mathf.hpp"
#include "npchar.hpp"
#include "dbcomid.hpp"
#include "serdesmsg.hpp"
#include "dbcomrecord.hpp"
#include "actormsgpack.hpp"

void NPChar::on_AM_ACTION(const ActorMsgPack &mpk)
{
    const auto amA = mpk.conv<AMAction>();
    switch(uidf::getUIDType(amA.UID)){
        case UID_PLY:
        case UID_MON:
            {
                dispatchAction(amA.UID, makeActionStand());
                break;
            }
        default:
            {
                break;
            }
    }
}

void NPChar::on_AM_NPCEVENT(const ActorMsgPack &mpk)
{
    if(!mpk.from()){
        throw fflerror("NPC event comes from zero uid");
    }

    const auto amNPCE = mpk.conv<AMNPCEvent>();

    // when CO initiatively sends a message to NPC, we assume it's UID is the sessionUID
    // when NPC querys CO attributes the response should be handled in actor response handler, not here

    if(false
            || std::string(amNPCE.event) == SYS_NPCDONE
            || std::string(amNPCE.event) == SYS_NPCERROR){
        m_luaModulePtr->close(mpk.from());
        return;
    }

    if(std::string(amNPCE.event) == SYS_NPCQUERY){
        throw fflerror("unexcepted NPC event: event = %s, value = %s", to_cstr(amNPCE.event), to_cstr(amNPCE.value));
    }

    // can be SYS_NPCINIT or scritp event
    // script event defines like text button pressed etc

    if(amNPCE.mapID != mapID() || mathf::LDistance2(amNPCE.x, amNPCE.y, X(), Y()) >= SYS_MAXNPCDISTANCE * SYS_MAXNPCDISTANCE){
        AMNPCError amNPCE;
        std::memset(&amNPCE, 0, sizeof(amNPCE));

        amNPCE.errorID = NPCE_TOOFAR;
        m_actorPod->forward(mpk.from(), {AM_NPCERROR, amNPCE});

        m_luaModulePtr->close(mpk.from());
        return;
    }
    m_luaModulePtr->setEvent(mpk.from(), mpk.from(), amNPCE.event, amNPCE.value);
}

void NPChar::on_AM_NOTIFYNEWCO(const ActorMsgPack &mpk)
{
    if(uidf::getUIDType(mpk.from()) == UID_PLY){
        dispatchAction(mpk.from(), makeActionStand());
    }
}

void NPChar::on_AM_QUERYCORECORD(const ActorMsgPack &mpk)
{
    const auto fromUID = mpk.conv<AMQueryCORecord>().UID;
    if(uidf::getUIDType(fromUID) != UID_PLY){
        throw fflerror("NPC get AMQueryCORecord from %s", uidf::getUIDTypeCStr(fromUID));
    }
    dispatchAction(fromUID, makeActionStand());
}

void NPChar::on_AM_QUERYLOCATION(const ActorMsgPack &mpk)
{
    AMLocation amL;
    std::memset(&amL, 0, sizeof(amL));

    amL.UID       = UID();
    amL.mapID     = mapID();
    amL.X         = X();
    amL.Y         = Y();
    amL.Direction = Direction();

    m_actorPod->forward(mpk.from(), {AM_LOCATION, amL}, mpk.seqID());
}

void NPChar::on_AM_QUERYSELLITEMLIST(const ActorMsgPack &mpk)
{
    const auto amQSIL = mpk.conv<AMQuerySellItemList>();
    SDSellItemList sdSIL;

    if(DBCOM_ITEMRECORD(amQSIL.itemID).packable()){
        if(auto p = m_sellItemList.begin(); p != m_sellItemList.end()){
            for(const auto &[seqID, sellItem]: p->second){
                sdSIL.list.push_back(SDSellItem
                {
                    .item = sellItem.item,
                    .costList = sellItem.costList,
                });
            }
        }
    }
    else{
        SDSellItem single;
        single.item = SDItem
        {
            .itemID = amQSIL.itemID,
            . seqID = 0,
        };

        single.costList.push_back(SDCostItem
        {
            .itemID = DBCOM_ITEMID(u8"金币"),
            .count  = (size_t)(80 + std::rand() % 20),
        });
        sdSIL.list.push_back(std::move(single));
    }
    sendNetPackage(mpk.from(), SM_SELLITEMLIST, cerealf::serialize(sdSIL, true));
}

void NPChar::on_AM_BADACTORPOD(const ActorMsgPack &mpk)
{
    const auto amBAP = mpk.conv<AMBadActorPod>();
    m_luaModulePtr->close(amBAP.UID);
}

void NPChar::on_AM_BUY(const ActorMsgPack &mpk)
{
    const auto fnSendBuyError = [&mpk, this](int buyError)
    {
        AMBuyError amBE;
        std::memset(&amBE, 0, sizeof(amBE));
        amBE.error = buyError;
        m_actorPod->forward(mpk.from(), {AM_BUYERROR, amBE});
    };

    const auto amB = mpk.conv<AMBuy>();
    auto p = m_sellItemList.find(amB.itemID);
    if(p == m_sellItemList.end()){
        fnSendBuyError(BUYERR_BADITEM);
        return;
    }

    auto q = p->second.find(amB.seqID);
    if(q == p->second.end()){
        fnSendBuyError(BUYERR_SOLDOUT);
        return;
    }

    if(q->second.locked){
        fnSendBuyError(BUYERR_LOCKED);
        return;
    }

    AMBuyCost amBC;
    std::memset(&amBC, 0, sizeof(amBC));
    if(std::extent_v<decltype(amBC.itemList)> < q->second.costList.size()){
        throw fflerror("too many exchange items: itemID = %llu", to_llu(amB.itemID));
    }

    for(size_t i = 0; const auto &item: q->second.costList){
        amBC.itemList[i].itemID = item.itemID;
        amBC.itemList[i].count  = item.count;
        i++;
    }

    q->second.locked = true;
    m_actorPod->forward(mpk.from(), {AM_BUYCOST, amBC}, mpk.seqID(), [amB, this](const ActorMsgPack &rmpk)
    {
        auto p = m_sellItemList.find(amB.itemID);
        if(p == m_sellItemList.end()){
            throw fflerror("item list released with locked item inside: %llu", to_llu(amB.itemID));
        }

        auto q = p->second.find(amB.seqID);
        if(q == p->second.end()){
            throw fflerror("can't find locked item: itemID = %llu, seqID = %llu", to_llu(amB.itemID), to_llu(amB.seqID));
        }

        if(!q->second.locked){
            throw fflerror("item lock released before get response");
        }

        q->second.locked = false;
        switch(rmpk.type()){
            case AM_OK:
                {
                    p->second.erase(q);
                    return;
                }
            case AM_ERROR:
                {
                    q->second.locked = false;
                    return;
                }
            default:
                {
                    return;
                }
        }
    });
}
