//
// Created by alex on 25.08.18.
//

#ifndef _DATA_CONTAINER_H
#define _DATA_CONTAINER_H

#include <unordered_map>
#include <string>
#include <limits>
#include <memory>
#include "basics.h"

/// get Data Container type ID with initialization
#define DCID0(X) DataContainer::getDCtypeIDandInit<X>()
/// get Data Container type ID
#define DCID(X) DataContainer::getDCtypeID<X>()
/// Data Container User Method
#define DCUM(ACTION_METHOD) \
	reinterpret_cast<void (thisCls::*)(tPDC)>(\
					&thisCls::ACTION_METHOD)

#define DEFINE_clsName(value) \
	static constexpr const char* clsName = #value

#define EMPTY_DC_DEFINITION(DC_CLS_NAME)\
	struct DC_CLS_NAME : public DataContainer\
	{\
		static constexpr const char* clsName = #DC_CLS_NAME;\
		DC_CLS_NAME() : DataContainer(clsName) {};\
	}

#define EMPTY_DC_DEFINITION_ONBASE(CLSNAME, BASECLS)\
	struct CLSNAME : public BASECLS\
	{\
		static constexpr const char* clsName = #CLSNAME;\
		CLSNAME() : BASECLS(clsName) {};\
	};

typedef uint16_t tDataType;
namespace
{
	const tDataType UNDEF_DATA_TYPE_ID(
			std::numeric_limits<tDataType>::max());
}

class DataContainer
{
public:
	DataContainer(const char* clsName)
	{
		type = DataContainer::addType(clsName);
	}

	virtual ~DataContainer() {};

	tDataType getType()
	{
		return type;
	}

	static tDataType addType(std::string clsName)
	{
		auto p = types.find(clsName);
		if (p == types.end())
		{
			tDataType newType = (tDataType)types.size();
			if (newType == std::numeric_limits<tDataType>::max() - 1)
				throw std::exception();
			types.emplace(clsName, newType);
			return newType;
		}
		else
		{
			return p->second;
		}

	}

	template <class DC>
	static tDataType getDCtypeID()
	{
		return types.at(DC::clsName);
//		auto p = types.find(DC::clsName);
//		if (p != types.end())
//			return p->second;
//		return UNDEF_DATA_TYPE_ID;
	}

	template <class DC>
	static tDataType getDCtypeIDandInit()
	{
		auto *dc = new DC();
		delete dc;
		return getDCtypeID<DC>();
	}

protected:
	tDataType type;

	static std::unordered_map<std::string, tDataType> types;
};

typedef std::shared_ptr<DataContainer> tPDC;

#endif //_DATA_CONTAINER_H
