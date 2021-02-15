/*
 * =====================================================================================
 *
 *       Filename: actormsg.hpp
 *        Created: 05/03/2016 13:19:07
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

#pragma once
#include <cstdint>
#include "actionnode.hpp"
#include "actordatapackage.hpp"

enum ActorMsgPackType: int
{
    AM_NONE = 0,
    AM_OK,
    AM_ERROR,
    AM_BADACTORPOD,
    AM_BADCHANNEL,
    AM_TIMEOUT,
    AM_UID,
    AM_PING,
    AM_LOGIN,
    AM_METRONOME,
    AM_TRYMOVE,
    AM_TRYSPACEMOVE,
    AM_MOVEOK,
    AM_SPACEMOVEOK,
    AM_TRYLEAVE,
    AM_LOGINOK,
    AM_ADDRESS,
    AM_LOGINQUERYDB,
    AM_SENDPACKAGE,
    AM_RECVPACKAGE,
    AM_ADDCHAROBJECT,
    AM_BINDCHANNEL,
    AM_ACTION,
    AM_PULLCOINFO,
    AM_QUERYMAPLIST,
    AM_MAPLIST,
    AM_MAPSWITCH,
    AM_MAPSWITCHOK,
    AM_TRYMAPSWITCH,
    AM_QUERYMAPUID,
    AM_QUERYLOCATION,
    AM_QUERYSELLITEMLIST,
    AM_LOCATION,
    AM_PATHFIND,
    AM_PATHFINDOK,
    AM_ATTACK,
    AM_UPDATEHP,
    AM_DEADFADEOUT,
    AM_QUERYCORECORD,
    AM_QUERYCOCOUNT,
    AM_QUERYPLAYERWLDESP,
    AM_COCOUNT,
    AM_EXP,
    AM_MISS,
    AM_NEWDROPITEM,
    AM_SHOWDROPITEM,
    AM_NOTIFYDEAD,
    AM_OFFLINE,
    AM_PICKUP,
    AM_PICKUPITEMIDLIST,
    AM_REMOVEGROUNDITEM,
    AM_CORECORD,
    AM_NOTIFYNEWCO,
    AM_CHECKMASTER,
    AM_QUERYMASTER,
    AM_QUERYFINALMASTER,
    AM_QUERYFRIENDTYPE,
    AM_FRIENDTYPE,
    AM_QUERYNAMECOLOR,
    AM_NAMECOLOR,
    AM_MASTERKILL,
    AM_MASTERHITTED,
    AM_NPCQUERY,
    AM_NPCEVENT,
    AM_NPCERROR,
    AM_BUY,
    AM_BUYCOST,
    AM_BUYERROR,
    AM_MAX,
};

struct AMBadActorPod
{
    int      Type;
    uint64_t from;
    uint32_t ID;
    uint32_t Respond;
    uint64_t UID;
};

struct AMBadChannel
{
    uint32_t channID;
};

struct AMTryLeave
{
    int X;
    int Y;
};

struct AMAddCharObject
{
    int type;

    int x;
    int y;
    uint32_t mapID;
    bool strictLoc;

    struct _monsterType
    {
        uint32_t monsterID;
        uint64_t masterUID;
    };

    struct _playerType
    {
        uint32_t DBID;
        uint32_t jobID;

        int level;
        int direction;
        uint32_t channID;
    };

    struct _NPCType
    {
        int direction;
        uint16_t NPCID;
    };

    union
    {
        _NPCType NPC;
        _playerType player;
        _monsterType monster;
    };

    struct sdBuf
    {
        char   data[256];
        size_t size;
    };

    sdBuf buf;
};

struct AMLogin
{
    uint32_t DBID;
    uint64_t UID;
    uint32_t SID;
    uint32_t mapID;
    uint64_t Key;

    int X;
    int Y;
};

struct AMTrySpaceMove
{
    uint64_t UID;

    int X;
    int Y;

    bool StrictMove;
};

struct AMSpaceMoveOK
{
    int X;
    int Y;
};

struct AMTryMove
{
    uint64_t UID;
    uint32_t mapID;

    int X;
    int Y;

    int EndX;
    int EndY;

    bool AllowHalfMove;
    bool RemoveMonster;
};

struct AMMoveOK
{
    uint64_t UID;
    uint32_t mapID;

    int X;
    int Y;

    int EndX;
    int EndY;
};

struct AMLoginQueryDB
{
    uint32_t channID;

    uint32_t DBID;
    uint32_t mapID;
    int      MapX;
    int      MapY;
    int      Level;
    int      JobID;
    int      Direction;
};

struct AMSendPackage
{
    ActorDataPackage package;
};

struct AMRecvPackage
{
    uint32_t channID;
    ActorDataPackage package;
};

struct AMBindChannel
{
    uint32_t channID;
};

struct AMAction
{
    uint64_t UID;
    uint32_t mapID;
    ActionNode action;
};

struct AMPullCOInfo
{
    int X;
    int Y;
    int W;
    int H;

    uint64_t UID;
    uint32_t mapID;
};

struct AMMapList
{
    uint32_t MapList[256];
};

struct AMMapSwitch
{
    uint32_t mapID;
    uint64_t UID;

    int X;
    int Y;
};

struct AMTryMapSwitch
{
    int X;
    int Y;
    bool strictMove;
};

struct AMQueryMapUID
{
    uint32_t mapID;
};

struct AMUID
{
    uint64_t UID;
};

struct AMMapSwitchOK
{
    void *Ptr;

    int X;
    int Y;
};

struct AMQueryLocation
{
    uint64_t UID;
    uint32_t mapID;
};

struct AMQuerySellItemList
{
    uint32_t itemID;
};

struct AMLocation
{
    uint64_t UID;
    uint32_t mapID;
    uint32_t RecordTime;

    int X;
    int Y;
    int Direction;
};

struct AMPathFind
{
    uint64_t UID;
    uint32_t mapID;

    int CheckCO;
    int MaxStep;

    int X;
    int Y;
    int EndX;
    int EndY;
};

struct AMPathFindOK
{
    uint64_t UID;
    uint32_t mapID;

    struct _Point
    {
        int X;
        int Y;
    }Point[8];
};

struct AMAttack
{
    uint64_t UID;
    uint32_t mapID;

    int X;
    int Y;

    int Type;
    int Damage;
    int Element;
    int Effect[32];
};

struct AMUpdateHP
{
    uint64_t UID;
    uint32_t mapID;

    int X;
    int Y;

    uint32_t HP;
    uint32_t HPMax;
};

struct AMDeadFadeOut
{
    uint64_t UID;
    uint32_t mapID;

    int X;
    int Y;
};

struct AMQueryCORecord
{
    uint64_t UID;
};

struct AMQueryCOCount
{
    uint32_t mapID;
    struct _Check
    {
        bool NPC;
        bool Player;
        bool Monster;
    }Check;

    struct _CheckParam
    {
        // initialize and use it as needed
        // collection to put all checking paramters here
        bool     Male;
        uint32_t MonsterID;
    }CheckParam;
};

struct AMCOCount
{
    uint32_t Count;
};

struct AMExp
{
    int Exp;
};

struct AMMiss
{
    uint64_t UID;
};

struct AMNewDropItem
{
    uint64_t UID;
    int X;
    int Y;

    uint32_t ID;
    int Value;
};

struct AMShowDropItem
{
    struct _CommonItem
    {
        uint32_t ID;
        uint32_t DBID;
    }IDList[16];

    int X;
    int Y;
};

struct AMNotifyDead
{
    uint64_t UID;
};

struct AMOffline
{
    uint64_t UID;
    uint32_t mapID;

    int X;
    int Y;
};

struct AMPickUp
{
    int x;
    int y;
    uint32_t availableWeight;
};

struct AMPickUpItemIDList
{
    uint32_t failedItemID;
    uint32_t itemIDList[SYS_MAXDROPITEM];
};

struct AMRemoveGroundItem
{
    int X;
    int Y;

    uint32_t ID;
    uint32_t DBID;
};

struct AMCORecord
{
    uint64_t UID;
    uint32_t mapID;
    ActionNode action;

    // instantiation of anonymous struct is supported in C11
    // not C++11, so we define structs outside of anonymous union

    struct _AMCORecord_Monster
    {
        uint32_t MonsterID;
    };

    struct _AMCORecord_Player
    {
        uint32_t Level;
    };

    struct _AMCORecord_NPC
    {
        uint32_t NPCID;
    };

    union
    {
        _AMCORecord_Monster Monster;
        _AMCORecord_Player  Player;
        _AMCORecord_NPC     NPC;
    };
};

struct AMNotifyNewCO
{
    uint64_t UID;
};

struct AMQueryFriendType
{
    uint64_t UID;
};

struct AMFriendType
{
    int Type;
};

struct AMNameColor
{
    int Color;
};

struct AMNPCEvent
{
    int x;
    int y;
    uint32_t mapID;

    char event[32];
    char value[32];
};

struct AMNPCQuery
{
    char query[32];
};

struct AMNPCError
{
    int errorID;
};

struct AMBuy
{
    uint32_t itemID;
    uint32_t seqID;
    size_t   count;
};

struct AMBuyCost
{
    struct ItemBuf
    {
        char   data[256];
        size_t size;
    };

    struct ItemListElement
    {
        uint32_t itemID;
        size_t   count;
    };

    ItemBuf itemBuf;
    ItemListElement itemList[8];
};

struct AMBuyError
{
    int error;
};
