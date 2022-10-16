#include "luaPage.h"
#include "Preset.h"
#include "MainDelegate.h"

extern Preset *luaPreset;
extern MainDelegate *luaDelegate;

static Page *getPage(lua_State *L, uint8_t stackPosition)
{
    return (
        *reinterpret_cast<Page **>(luaL_checkudata(L, stackPosition, "Page")));
}

int luaopen_pages(lua_State *L)
{
    luaL_newlib(L, pages_functions);
    return 1;
}

int pages_get(lua_State *L)
{
    lua_settop(L, 1);
    int pageId = luaLE_checkPageId(L, 1);

    const Page &page = luaPreset->getPage(pageId);

    if (page.isValid()) {
        luaLE_pushObject(L, "Page", &page);
        return (1);
    }
    return (luaL_error(L, "failed: page %d does not exist", pageId));
}

int pages_getActive(lua_State *L)
{
    lua_settop(L, 0);

    int pageId = luaDelegate->getCurrentPageId();
    const Page &page = luaPreset->getPage(pageId);

    if (page.isValid()) {
        luaLE_pushObject(L, "Page", &page);
        return (1);
    }
    return (luaL_error(L, "failed: page %d does not exist", pageId));
}

int pages_display(lua_State *L)
{
    int numParameters = lua_gettop(L);
    int pageId = 1;
    int controlSetId = 0;

    if (numParameters > 0) {
        pageId = luaLE_checkPageId(L, 1);
    }

    if (numParameters > 1) {
        controlSetId = luaLE_checkControlSetId(L, 2);
    }

    luaDelegate->switchPage(pageId, controlSetId); // freezes E1
    return (0);
}

int pages_setActiveControlSet(lua_State *L)
{
    lua_settop(L, 1);
    int controlSetId = luaLE_checkControlSetId(L, 1);
    luaDelegate->switchControlSet(controlSetId); // freezes E1
    return (0);
}

int pages_getActiveControlSet(lua_State *L)
{
    lua_settop(L, 0);
    lua_pushinteger(L, luaDelegate->getCurrentControlSetId());

    return (1);
}

int page_delete(lua_State *L)
{
    lua_settop(L, 1);
    return (0);
}

int page_getId(lua_State *L)
{
    lua_settop(L, 1);
    int id = 0;

    if (Page *page = getPage(L, 1)) {
        id = page->getId();
        lua_pushinteger(L, id);
        return (1);
    }
    return (luaL_error(L, "failed: not a valid page"));
}

int page_setName(lua_State *L)
{
    lua_settop(L, 2);

    Page *page = getPage(L, 1);
    const char *name = luaL_checkstring(L, 2);

    if (page) {
        luaDelegate->setPageName(page->getId(), name);
    } else {
        return (luaL_error(L, "failed: not a valid page"));
    }
    return (0);
}

int page_getName(lua_State *L)
{
    lua_settop(L, 1);

    if (Page *page = getPage(L, 1)) {
        lua_pushstring(L, page->getName());
        return (1);
    }
    return (luaL_error(L, "failed: not a valid page"));
}

void page_register(lua_State *L)
{
    lua_register(L, "Page", pages_get);
    luaL_newmetatable(L, "Page");

    lua_pushcfunction(L, page_delete);
    lua_setfield(L, -2, "__gc");

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luaLE_addObjectMethods(L, page_functions);

    lua_pop(L, 1);
}
