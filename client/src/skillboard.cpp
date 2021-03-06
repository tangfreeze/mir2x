/*
 * =====================================================================================
 *
 *       Filename: skillboard.cpp
 *        Created: 10/08/2017 19:22:30
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

#include "dbcomid.hpp"
#include "pngtexdb.hpp"
#include "sdldevice.hpp"
#include "processrun.hpp"
#include "skillboard.hpp"
#include "dbcomrecord.hpp"
#include "tritexbutton.hpp"

extern PNGTexDB *g_progUseDB;
extern SDLDevice *g_sdlDevice;

SkillBoard::MagicIconButton::MagicIconButton(int argX, int argY, SkillBoard::MagicIconData *iconDataPtr, Widget *widgetPtr, bool autoDelete)
    : Widget
      {
          argX,
          argY,
          0,
          0,
          widgetPtr,
          autoDelete,
      }

    , m_magicIconDataPtr([iconDataPtr]()
      {
          if(!iconDataPtr){
              throw fflerror("invalid iconDataPtr: nullptr");
          }
          return iconDataPtr;
      }())

    , m_key
      {
          2,
          2,
          2,
          2,
          u8"",

          3,
          20,
          0,

          colorf::RGBA(0XFF, 0X80, 0X00, 0XE0),
          colorf::RGBA(0X00, 0X00, 0X00, 0XE0),

          this,
          false,
      }

    , m_level
      {
          0,
          0,
          u8"",

          3,
          12,
          0,

          colorf::RGBA(0XFF, 0XFF, 0X00, 0XFF),
          this,
      }

    , m_icon
      {
          0,
          0,

          {
              DBCOM_MAGICRECORD(m_magicIconDataPtr->magicID).icon,
              DBCOM_MAGICRECORD(m_magicIconDataPtr->magicID).icon,
              DBCOM_MAGICRECORD(m_magicIconDataPtr->magicID).icon,
          },

          nullptr,
          nullptr,
          nullptr,

          0,
          0,
          0,
          0,

          false,
          false,
          this,
          false,
      }
{
    setKey(m_magicIconDataPtr->key);
    setLevel(m_magicIconDataPtr->level);

    m_level.moveTo(m_icon.w() - 2, m_icon.h() - 1);
    m_w = m_level.dx() + m_level.w();
    m_h = m_level.dy() + m_level.h();
}

void SkillBoard::MagicIconButton::drawEx(int dstX, int dstY, int srcX, int srcY, int srcW, int srcH) const
{
    // to make sure the key always draw on top
    // otherwise WidgetGroup changes draw order when triggers icon callback, check WidgetGroup::drawEx()

    const auto fnDrawWidget = [dstX, dstY, srcX, srcY, srcW, srcH, this](const Widget *widgetPtr)
    {
        if(!widgetPtr->show()){
            return;
        }

        int srcXCrop = srcX;
        int srcYCrop = srcY;
        int dstXCrop = dstX;
        int dstYCrop = dstY;
        int srcWCrop = srcW;
        int srcHCrop = srcH;

        if(!mathf::ROICrop(
                    &srcXCrop, &srcYCrop,
                    &srcWCrop, &srcHCrop,
                    &dstXCrop, &dstYCrop,

                    w(),
                    h(),

                    widgetPtr->dx(), widgetPtr->dy(), widgetPtr->w(), widgetPtr->h())){
            return;
        }
        widgetPtr->drawEx(dstXCrop, dstYCrop, srcXCrop - widgetPtr->dx(), srcYCrop - widgetPtr->dy(), srcWCrop, srcHCrop);
    };

    fnDrawWidget(&m_icon);
    fnDrawWidget(&m_key);
    fnDrawWidget(&m_level);
}

SkillBoard::SkillPage::SkillPage(uint32_t pageImage, Widget *widgetPtr, bool autoDelete)
    : WidgetGroup
      {
          SkillBoard::getPageRectange().at(0),
          SkillBoard::getPageRectange().at(1),
          0, // reset in ctor body
          0,
          widgetPtr,
          autoDelete,
      }
    , m_pageImage(pageImage)
{
    std::tie(m_w, m_h) = [this]() -> std::tuple<int, int>
    {
        if(auto texPtr = g_progUseDB->Retrieve(m_pageImage)){
            return SDLDeviceHelper::getTextureSize(texPtr);
        }

        const auto r = SkillBoard::getPageRectange();
        return {r[2], r[3]};
    }();
}

void SkillBoard::SkillPage::drawEx(int dstX, int dstY, int srcX, int srcY, int srcW, int srcH) const
{
    if(auto texPtr = g_progUseDB->Retrieve(m_pageImage)){
        int srcXCrop = srcX;
        int srcYCrop = srcY;
        int dstXCrop = dstX;
        int dstYCrop = dstY;
        int srcWCrop = srcW;
        int srcHCrop = srcH;

        const auto [texW, texH] = SDLDeviceHelper::getTextureSize(texPtr);
        if(mathf::ROICrop(
                    &srcXCrop, &srcYCrop,
                    &srcWCrop, &srcHCrop,
                    &dstXCrop, &dstYCrop,

                    texW,
                    texH)){
            g_sdlDevice->drawTexture(texPtr, dstXCrop, dstYCrop, srcXCrop, srcYCrop, srcWCrop, srcHCrop);
        }
    }
    WidgetGroup::drawEx(dstX, dstY, srcX, srcY, srcW, srcH);
}

SkillBoard::SkillBoard(int nX, int nY, ProcessRun *runPtr, Widget *pwidget, bool autoDelete)
    : Widget(nX, nY, 0, 0, pwidget, autoDelete)
    , m_magicIconDataList
      {
          {DBCOM_MAGICID(u8"雷电术"),   1,  12,  78,  'T'},
          {DBCOM_MAGICID(u8"魔法盾"),   2, 252, 143,  'Y'},
          {DBCOM_MAGICID(u8"召唤骷髅"), 3,  12,  13,  'U'},
          {DBCOM_MAGICID(u8"召唤神兽"), 1,  12,  78,  'S'},
          {DBCOM_MAGICID(u8"灵魂火符"), 2, 192,  13,  'F'},
      }

    , m_skillPageList([this]() -> std::vector<SkillBoard::SkillPage *>
      {
          std::vector<SkillBoard::SkillPage *> pageList;
          pageList.reserve(8);

          for(int i = 0; i < 8; ++i){
              auto pagePtr = new SkillBoard::SkillPage
              {
                  to_u32(0X05000010 + (uint32_t)(i)),
                  this,
                  true,
              };

              for(auto &iconRef: m_magicIconDataList){
                  if(i == getSkillPageIndex(iconRef.magicID)){
                      pagePtr->addIcon(&iconRef);
                  }
              }
              pageList.push_back(pagePtr);
          }
          return pageList;
      }())

    , m_tabButtonList([this]() -> std::vector<TritexButton *>
      {
          std::vector<TritexButton *> tabButtonList;
          tabButtonList.reserve(8);

          const int tabX = 45;
          const int tabY = 10;
          const int tabW = 34;

          // don't know WTF
          // can't compile if use std::vector<TritexButton>
          // looks for complicated parameter list gcc can't forward correctly ???

          for(int i = 0; i < 8; ++i){
              tabButtonList.push_back(new TritexButton
              {
                  tabX + tabW * i,
                  tabY,

                  {
                      SYS_TEXNIL,
                      0X05000020 + (uint32_t)(i),
                      0X05000030 + (uint32_t)(i),
                  },

                  [i, this]()
                  {
                      m_cursorOnTabIndex = i;
                  },

                  [i, this]()
                  {
                      if(i != m_cursorOnTabIndex){
                          return;
                      }
                      m_cursorOnTabIndex = -1;
                  },

                  [i, this]()
                  {
                      if(m_selectedTabIndex == i){
                          return;
                      }

                      m_tabButtonList.at(i)->setTexID(
                      {
                          0X05000030 + (uint32_t)(i),
                          0X05000030 + (uint32_t)(i),
                          0X05000030 + (uint32_t)(i),
                      });

                      m_tabButtonList.at(m_selectedTabIndex)->setOff();
                      m_tabButtonList.at(m_selectedTabIndex)->setTexID(
                      {
                          SYS_TEXNIL,
                          0X05000020 + (uint32_t)(m_selectedTabIndex),
                          0X05000030 + (uint32_t)(m_selectedTabIndex),
                      });

                      m_selectedTabIndex = i;
                      m_slider.setValue(0);

                      const auto r = getPageRectange();
                      for(auto pagePtr: m_skillPageList){
                          pagePtr->moveTo(r[0], r[1]);
                      }
                  },

                  0,
                  0,
                  0,
                  0,

                  false,
                  false,
                  this,
                  true,
              });

              if(i == m_selectedTabIndex){
                  m_tabButtonList.at(i)->setTexID(
                  {
                      0X05000030 + (uint32_t)(i),
                      0X05000030 + (uint32_t)(i),
                      0X05000030 + (uint32_t)(i),
                  });
              }
          }
          return tabButtonList;
      }())

    , m_slider
      {
          326,
          74,

          266,
          2,

          [this](float value)
          {
              const auto r = SkillBoard::getPageRectange();
              auto pagePtr = m_skillPageList.at(m_selectedTabIndex);

              if(r[3] < pagePtr->h()){
                  pagePtr->moveTo(r[0], r[1] - (pagePtr->h() - r[3]) * value);
              }
          },
          this,
      }

    , m_closeButton
      {
          317,
          402,
          {SYS_TEXNIL, 0X0000001C, 0X0000001D},

          nullptr,
          nullptr,
          [this]()
          {
              show(false);
          },

          0,
          0,
          0,
          0,

          true,
          true,
          this,
      }
    , m_processRun(runPtr)
{
    show(false);
    if(auto texPtr = g_progUseDB->Retrieve(0X05000000)){
        std::tie(m_w, m_h) = SDLDeviceHelper::getTextureSize(texPtr);
    }
    else{
        throw fflerror("no valid inventory frame texture");
    }
}

void SkillBoard::drawEx(int dstX, int dstY, int, int, int, int) const
{
    if(auto texPtr = g_progUseDB->Retrieve(0X05000000)){
        g_sdlDevice->drawTexture(texPtr, dstX, dstY);
    }

    drawTabName();
    m_slider.draw();
    m_closeButton.draw();

    for(auto buttonPtr: m_tabButtonList){
        buttonPtr->draw();
    }

    const auto r = SkillBoard::getPageRectange();
    auto pagePtr = m_skillPageList.at(m_selectedTabIndex);
    pagePtr->drawEx(dstX + r[0], dstY + r[1], r[0] - pagePtr->dx(), r[1] - pagePtr->dy(), r[2], r[3]);
}

void SkillBoard::update(double)
{
}

bool SkillBoard::processEvent(const SDL_Event &event, bool valid)
{
    if(!valid){
        return focusConsume(this, false);
    }

    if(!show()){
        return focusConsume(this, false);
    }

    if(m_closeButton.processEvent(event, valid)){
        return focusConsume(this, show());
    }

    bool tabConsumed = false;
    for(auto buttonPtr: m_tabButtonList){
        tabConsumed |= buttonPtr->processEvent(event, valid && !tabConsumed);
    }

    if(tabConsumed){
        return focusConsume(this, true);
    }

    if(m_slider.processEvent(event, valid)){
        return focusConsume(this, true);
    }

    const auto r = getPageRectange();
    const auto loc = SDLDeviceHelper::getEventPLoc(event);
    const bool captureEvent = loc && mathf::pointInRectangle(loc.x, loc.y, x() + r[0], y() + r[1], r[2], r[3]);

    if(m_skillPageList.at(m_selectedTabIndex)->processEvent(event, captureEvent && valid)){
        return focusConsume(this, true);
    }

    switch(event.type){
        case SDL_MOUSEMOTION:
            {
                if((event.motion.state & SDL_BUTTON_LMASK) && (in(event.motion.x, event.motion.y) || focus())){
                    const auto [rendererW, rendererH] = g_sdlDevice->getRendererSize();
                    const int maxX = rendererW - w();
                    const int maxY = rendererH - h();

                    const int newX = std::max<int>(0, std::min<int>(maxX, x() + event.motion.xrel));
                    const int newY = std::max<int>(0, std::min<int>(maxY, y() + event.motion.yrel));

                    moveBy(newX - x(), newY - y());
                    return focusConsume(this, true);
                }
                return focusConsume(this, false);
            }
        case SDL_MOUSEBUTTONDOWN:
            {
                switch(event.button.button){
                    case SDL_BUTTON_LEFT:
                        {
                            return focusConsume(this, in(event.button.x, event.button.y));
                        }
                    default:
                        {
                            return focusConsume(this, false);
                        }
                }
            }
        default:
            {
                return focusConsume(this, false);
            }
    }
}

uint32_t SkillBoard::key2MagicID(char key)
{
    for(const auto &iconCRef: m_magicIconDataList){
        if(std::tolower(iconCRef.key) == std::tolower(key)){
            return iconCRef.magicID;
        }
    }
    return 0;
}

void SkillBoard::drawTabName() const
{
    const LabelBoard tabName
    {
        0,
        0,

        to_u8cstr([this]() -> std::u8string
        {
            if(m_cursorOnTabIndex >= 0){
                return str_printf(u8"元素【%s】", to_cstr(magicElemName(cursorOnElem())));
            }

            if(m_selectedTabIndex >= 0){
                for(const auto magicIconPtr: m_skillPageList.at(m_selectedTabIndex)->getMagicIconButtonList()){
                    if(magicIconPtr->cursorOn()){
                        if(const auto &mr = DBCOM_MAGICRECORD(magicIconPtr->getMagicIconDataPtr()->magicID)){
                            return str_printf(u8"元素【%s】%s", to_cstr(mr.elem), to_cstr(mr.name));
                        }
                        else{
                            return str_printf(u8"元素【无】");
                        }
                    }
                }
                return str_printf(u8"元素【%s】", to_cstr(magicElemName(selectedElem())));
            }

            // fallback
            // shouldn't reach here
            return str_printf(u8"元素【无】");
        }()),

        1,
        12,
        0,

        colorf::WHITE + 255,
    };
    tabName.drawAt(DIR_UPLEFT, x() + 30, y() + 400);
}
