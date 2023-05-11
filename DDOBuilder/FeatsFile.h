// FeatsFile.h
//

#pragma once
#include "XmlLib\SaxContentElement.h"
#include "XmlLib\SaxWriter.h"
#include "Feat.h"
#include <map>

class FeatsFile :
    public XmlLib::SaxContentElement
{
    public:
        FeatsFile(const std::string& filename);
        ~FeatsFile(void);

        void Read();
        std::map<std::string, Feat> Feats() const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        void EndElement();

        std::string m_filename;
        std::list<Feat> m_loadedFeats;
        size_t m_loadTotal;
};

