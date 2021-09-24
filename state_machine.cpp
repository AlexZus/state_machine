//
// Created by alex on 26.06.18.
//

#include "state_machine.hpp"
#include "data_container.hpp"

void StateMapCreator::add(tState state, tState newState,
						  ActionFunc actionFunc)
{
	for (auto t : currentActionsData)
		actionsMap[t][state] = ActionCore{actionFunc, newState};
};

void StateMapCreator::add(tState state)
{
	for (auto t : currentActionsData)
		actionsMap[t][state] =
			ActionCore{&StateMachine::act_nothing, state};
};

std::unordered_map<tDataType, ActionCore*> StateMapCreator::initMap()
{
	std::unordered_map<tDataType, ActionCore*> res;
	for (auto pa = actionsMap.begin(); pa != actionsMap.end(); ++pa)
	{
		auto action = pa->first;
		res[action] = new ActionCore[statesNum];
		auto statesMap = pa->second;
		assert(statesMap.size() == statesNum);
		for (auto ps = statesMap.begin(); ps != statesMap.end(); ++ps)
		{
			auto state = ps->first;
			assert(state < statesNum);
			res[action][state] = ps->second;
		}
	}
	return res;
}


StateMachine::StateMachine(tState initialState) :
		currentState(initialState)
{
	eventData = nullptr;
	statesNum = 0;
}

StateMachine::StateMachine(const StateMachine& sm)
{
	copyThis(sm);
}

StateMachine::StateMachine(StateMachine&& sm)
{
	moveThis(sm);
}

StateMachine& StateMachine::operator=(const StateMachine& sm)
{
	if (this == &sm)
		return *this;
	deleteThis();
	copyThis(sm);
	return *this;
}

StateMachine& StateMachine::operator=(StateMachine&& sm)
{
	if (this == &sm)
		return *this;
	deleteThis();
	moveThis(sm);
	return *this;
}

void StateMachine::copyThis(const StateMachine& sm)
{
	isEvent = false;
	eventData = nullptr;

	currentState = sm.currentState;

	statesNum = sm.statesNum;
	stateMap.clear();
	for (auto& p : sm.stateMap)
	{
		stateMap[p.first] = new ActionCore[statesNum];
		for (size_t s = 0; s < statesNum; ++s)
		{
			stateMap.at(p.first)[s] = p.second[s];
		}
	}
}

void StateMachine::moveThis(StateMachine& sm)
{
	isEvent = sm.isEvent;
	sm.isEvent = false;
	eventActionCore = sm.eventActionCore;
	eventData = sm.eventData;
	sm.eventData = nullptr;

	currentState = sm.currentState;

	statesNum = sm.statesNum;
	sm.statesNum = 0;
	stateMap = sm.stateMap;
	for (auto& p : sm.stateMap)
		p.second = nullptr;
}

StateMachine::~StateMachine()
{
	deleteThis();
}

void StateMachine::deleteThis()
{
	for (auto& p : stateMap)
	{
		delete[] p.second;
	}
	stateMap.clear();
}

bool StateMachine::isCanAct(tDataType dataType)
{
	return stateMap.find(dataType) != stateMap.end();
}

bool StateMachine::doAction(tPDC data)
{
	ActionCore actionCore = stateMap.at(data->getType())[currentState];
	if (actionCore.resultState == currentState &&
		actionCore.actionFunc == &StateMachine::act_nothing)
	{
		return false;
	}
	internalAction(actionCore, data);
	process();
	return true;
};

bool StateMachine::doAction(tDataType dataType)
{
	ActionCore actionCore = stateMap.at(dataType)[currentState];
	if (actionCore.resultState == currentState &&
		actionCore.actionFunc == &StateMachine::act_nothing)
	{
		return false;
	}
	internalAction(actionCore, nullptr);
	process();
	return true;
};

void StateMachine::process()
{
	tPDC pDataTemp = nullptr;
	while (isEvent)
	{
		pDataTemp = eventData;
		eventData = nullptr;
		isEvent = false;

		auto newState = eventActionCore.resultState;
		(this->*eventActionCore.actionFunc)(pDataTemp);
		currentState = newState;
	}
}

void StateMachine::initStateMap(StateMapCreator& stateMapCreator)
{
	stateMap = stateMapCreator.initMap();
	statesNum = stateMapCreator.statesNum;
}

void StateMachine::internalAction(ActionCore actionCore, tPDC data)
{
	isEvent = true;
	eventData = data;
	eventActionCore = actionCore;
}

void StateMachine::trigToState(tState newState)
{
	internalAction(ActionCore{&StateMachine::act_nothing, newState});
}
