/** this is a loader of properties for various UI objects according to a skin
 *  file
 */
#include "mikaSkinEx.h"

mikaSkinEx *globalSkin = NULL;

mikaSkinEx::mikaSkinEx( )
{
    #ifdef _DEBUG
        printf("mikaSkinEx::mikaSkinEx\n");
    #endif
    
    doc = new TiXmlDocument((const char*)_SKIN_PATH);
    if (doc)
    {
        doc->LoadFile();
    }
}

mikaSkinEx::~mikaSkinEx( )
{
    
}

/** process first level widget types. To use sublevel objects of the setup,
 *  please use the mika* classes
 *
 */
int mikaSkinEx::setupWidget( tr_Object *w, unsigned char *name )
{
    #ifdef _DEBUG
        printf("mikaSkinEx::setupWidget\n");
    #endif
    if (!w || !name)
    {
        #ifdef _DEBUG
            printf("mikaSkinEx::setupWidget error, invalid arguments\n");
        #endif
        return 1;
    }
    
    if (doc)
    {
        TiXmlElement *root = doc->FirstChildElement("mikaSkin");
        if (root)
        {
            TiXmlElement *wo = root->FirstChildElement((char*)name);
            if (wo)
            {
                char *color = (char*)wo->Attribute("color");
                if (color)
                    tr_SetColor(w,color);
                
                
                char *fontcolor = (char*)wo->Attribute("fontcolor");
                if (fontcolor)
                    tr_SetFontColorEx(w,fontcolor);
                
                char *fontsize = (char*)wo->Attribute("fontsize");
                if (fontsize)
                    tr_SetFontSize(w,atoi(fontsize));
            }
            else
            {
                #ifdef _DEBUG
                    printf("mikaSkinEx::setupWidget error, setup not found for(%s)\n",
                        (char*)name);
                #endif
                return 1;
            }
        }
    }
    
    return 0;
}

mikaSkinEx *getSkin( )
{
    #ifdef _DEBUG
        printf("getSkin\n");
    #endif
    if (!globalSkin)
        globalSkin = new mikaSkinEx();
    return globalSkin;
}
