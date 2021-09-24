//
// Created by alex on 26.06.18.
//

#ifndef _STATE_MACHINE_HPP
#define _STATE_MACHINE_HPP

#include <functional>
#include <cassert>
#include <unordered_map>
#include <vector>
#include "data_container.hpp"

class StateMachine;
typedef void (StateMachine::*ActionFunc)(tPDC);
typedef unsigned char tState;

#define SMA(ACTION_METHOD) \
	reinterpret_cast<ActionFunc>(\
	&thisCls::ACTION_METHOD)
#define DCP tPDC pDC

struct ActionCore
{
	ActionFunc actionFunc;
	tState resultState;
};

class StateMapCreator
{
public:
	StateMapCreator(tState statesNum)
			: statesNum(statesNum)
	{};

	void useAction(tDataType actionData)
	{ currentActionsData = {actionData}; };

	void useAction(std::vector<tDataType> actionsData)
	{
		currentActionsData = actionsData;
	};

	void add(tState state, tState newState, ActionFunc actionFunc);
	void add(tState state);

	std::unordered_map<tDataType, ActionCore*> initMap();

public:
	const tState statesNum;
private:
	std::vector<tDataType> currentActionsData;
	std::unordered_map<tDataType, std::unordered_map<tState, ActionCore> >
			actionsMap;
};

class StateMachine
{
public:
	StateMachine(tState initialState);

	StateMachine(const StateMachine& f); // конструктор копирования

	StateMachine(StateMachine&& f); // конструктор переноса

	StateMachine& operator=(const StateMachine& f); // присваивание с копированием

	StateMachine& operator=(StateMachine&& f); // присваивание с переносом

	virtual ~StateMachine();

	bool isCanAct(tDataType dataType);

	bool doAction(tPDC data);

	bool doAction(tDataType dataType);

	void act_nothing(tPDC pData) {};

protected:
	void process();

	void deleteThis();

	void copyThis(const StateMachine& sm);

	void moveThis(StateMachine& sm);

	void initStateMap(StateMapCreator& stateMapCreator);

	void internalAction(ActionCore actionCore, tPDC data = nullptr);

	void trigToState(tState newState);

protected:
	tState currentState;

private:
	bool isEvent = false;
	tPDC eventData;
	ActionCore eventActionCore;
	std::unordered_map<tDataType, ActionCore*> stateMap;
	tState statesNum;
};

#endif //_STATE_MACHINE_HPP
