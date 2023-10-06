#include "mikaListData.h"

#define _LIST_DATA_DELEGATE

mikaListData::mikaListData( unsigned char *dataFile )
{
    #ifdef _DEBUG
        printf("mikaListData::mikaListData\n");
    #endif
    currentNode = NULL;
    delegate = NULL;
    doc = new TiXmlDocument((char*)dataFile);
    if (doc)
    {
        doc->LoadFile();
        TiXmlElement *eroot = doc->FirstChildElement("list");
        if (eroot)
        {
            TiXmlElement *enodes = eroot->FirstChildElement("nodes");
            if (enodes)
            {
                currentNode = enodes;
                listCurrent();
                #ifdef _LIST_DATA_DELEGATE
                    delegateNode();
                #endif
            }
        }
    }
}

mikaListData::~mikaListData( )
{
    
}

int mikaListData::back( )
{
    #ifdef _DEBUG
        printf("mikaListData::back, current:\n");
        listCurrent();
    #endif
    TiXmlElement *parent = (TiXmlElement*)currentNode->Parent();
    if (parent)
    {
        parent = (TiXmlElement*)parent->Parent();
        if (parent)
        {
            if (!parent->FirstChildElement("node"))
            {
                #ifdef _DEBUG
                    printf("\tmikaListData::back can't move\n");
                #endif
                delegateNode();
                return 0;
            }
            #ifdef _DEBUG
                printf("\tmikaListData::back moving\n");
            #endif
            currentNode = parent;
            listCurrent();
            #ifdef _LIST_DATA_DELEGATE
                delegateNode();
            #endif
        }
    }
    return 0;
}

/** retrieve the <nodes> of an element in position object is retrieved
 *  and if the <nodes> does not exist, the currentNode is retrieved
 */
TiXmlElement *mikaListData::elementAtPositionNodes( int pos )
{
    #ifdef _DEBUG
        printf("mikaListData::elementAtPosition\n");
    #endif
    if (pos<0)
        return NULL;
    if (!currentNode)
    {
        #ifdef _DEBUG
            printf("mikaListData::elementAtPosition error, no current\n");
        #endif
        return NULL;
    }
    
    TiXmlElement *node = elementAtPosition(pos);
    if (node)
    {
        TiXmlElement *nodes = node->FirstChildElement("nodes");
        if (nodes)
            return nodes;
        else
            return NULL;
    }
    
    return NULL;
}

TiXmlElement *mikaListData::elementAtPosition( int pos )
{
    if (!currentNode)
        return NULL;
    
    int count = 0;
    TiXmlElement *node = currentNode->FirstChildElement("node");
    if (node)
    {
        #ifdef _DEBUG
            printf("mikaListData::elementAtPosition check nodes\n\n");
        #endif
        if (pos==count)
        {
            return node;
        }
        count++;
        while(node = node->NextSiblingElement())
        {
            if (count==pos)
            {
                return node;
            }
            count++;
        }
    }

    #ifdef _DEBUG
        printf("mikaListData::elementAtPosition error, couldn't find element at pso(%d)\n",
            pos);
    #endif
    return NULL;
}

/** move to an element at the position pos
 *
 */
int mikaListData::moveToElementAtPosition( int pos )
{
    #ifdef _DEBUG
        printf("mikaListData::moveToElementAtPosition\n");
    #endif
    if (pos<0)
    {
        #ifdef _DEBUG
            printf("mikaListData::moveToElementAtPosition error, invalid position\n");
        #endif
        return 1;
    }
    
    TiXmlElement *element = elementAtPosition(pos);
    if (element)
    {
        TiXmlElement *nodes = elementAtPositionNodes(pos);
        if (nodes)
            currentNode = nodes;
        else
        {
            // wanted to move to the lowest element in tree
            #ifdef _DEBUG
                printf("\tmikaListData::moveToElementAtPosition, lowest element\n");
            #endif
            if (delegate)
            {
                if (delegate->onLowestElement)
                {
                    /** we can't dive into item, it has no children
                     */
                    delegate->onLowestElement(delegate,element,pos);
                    /** return here, since we don't want our listener to think to
                     *  to redraw items in a canvas, which doesn't need
                     *  to be redrawn
                     */
                    return 0;
                }
            }
        }    
        
        listCurrent();
        #ifdef _LIST_DATA_DELEGATE
            delegateNode();
        #endif
    }
    else
    {
        #ifdef _DEBUG
            printf("mikaListData::moveToElementAtPosition error, couldn't find element\n");
        #endif
        listCurrent();
        #ifdef _LIST_DATA_DELEGATE
            delegateNode();
        #endif
        return 1;
    }
    return 0;
}

int mikaListData::listCurrent( )
{
    #ifdef _DEBUG
        printf("mikaListData::listCurrent\n");
    #endif
    
    if (!currentNode)
    {
        #ifdef _DEBUG
            printf("mikaListData::listCurrent error, no current node defined\n");
        #endif
        return 1;
    }
    int count = 0;
    TiXmlElement *element = currentNode->FirstChildElement("node");
    if (element)
    {
        if (element->Attribute("name"))
        {
            #ifdef _DEBUG
                printf("mikaListData::listCurrent position(%d) name(%s)\n",
                    count,(char*)element->Attribute("name"));
            #endif
        }
        count++;
        while(element = element->NextSiblingElement())
        {
            if (element->Attribute("name"))
            {
                #ifdef _DEBUG
                    printf("mikaListData::listCurrent position(%d) name(%s)\n",
                        count,(char*)element->Attribute("name"));
                #endif
            }
            count++;
        }
    }
    #ifdef _DEBUG
        printf("mikaListData::listCurrent finish\n");
    #endif
    
    return 0;
}

/** delegate the information of the current node's elements
 *  This needs to be run whenever we move in the tree or update
 *  the data.
 */
int mikaListData::delegateNode( )
{
    #ifdef _DEBUG
        printf("mikaListData::delegateNode\n");
    #endif
    if (!delegate)
    {
        #ifdef _DEBUG
            printf("mikaListData::delegateNode error, no delegate\n");
        #endif
        return 1;
    }
    if (!delegate->onNodeElement)
        return 1;
    int count = 0;
    if (currentNode)
    {
        TiXmlElement *element = currentNode->FirstChildElement("node");
        if (element)
        {
            delegate->onNodeElement(delegate,element,count);
            count++;
            while(element = element->NextSiblingElement())
            {
                delegate->onNodeElement(delegate,element,count);
                count++;
            }
        }
    }
    #ifdef _DEBUG
        printf("mikaListData::delegateNode finish\n");
    #endif
    return 0;
}

/** retrieve the number of elements in current node
 */
int mikaListData::numberOfItems( )
{
    #ifdef _DEBUG
        printf("mikaListData::numberOfItems\n");
    #endif
    int num = 0;
    if (!currentNode)
        return 0;
    TiXmlElement *node = currentNode->FirstChildElement("node");
    if (node)
    {
        num++;
        while(node = node->NextSiblingElement())
        {
            num++;
        }
    }
    
    return num;
}
