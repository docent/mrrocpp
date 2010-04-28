/*
 * OrDataCondition.cc
 *
 *  Created on: Apr 28, 2010
 *      Author: ptroja
 */

#include <ostream>

#include <boost/foreach.hpp>

#include "OrDataCondition.h"
#include "AndDataCondition.h"

OrDataCondition::OrDataCondition()
{
}

OrDataCondition::OrDataCondition(const DataBufferBase &op)
{
	AndDataCondition cont(op);
	this->push_back(cont);
}

OrDataCondition::OrDataCondition(const AndDataCondition &op)
{
	this->push_back(op);
}

bool OrDataCondition::isNewData() const
{
	BOOST_FOREACH(const AndDataCondition & andCondition, *this) {
		if (andCondition.isNewData())
			return true;
	}
	return false;
}

OrDataCondition & OrDataCondition::operator=(const DataBufferBase &op)
{
	this->clear();
	AndDataCondition cont(op);
	this->push_back(cont);
	return *this;
}

std::ostream& operator<<(std::ostream& output, const OrDataCondition& me)
{
	for(std::size_t i = 0; i < me.size(); ++i) {
		const AndDataCondition & element = me.operator[](i);
		if(element.size() > 1) {
			output << "(" << element << ")";
		} else {
			output << element;
		}
		if(i < me.size() - 1) {
			output << " | ";
		}
	}

	return output;
}
