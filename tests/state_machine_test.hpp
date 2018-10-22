//
// Created by alex on 28.06.18.
//

#ifndef PAIDAGOGOS_STATE_MACHINE_TEST_H
#define PAIDAGOGOS_STATE_MACHINE_TEST_H

#include "state_machine.hpp"
#include "data_container.hpp"

namespace StateMachineTest
{
	struct DC_Set : public DataContainer
	{
		static constexpr const char* clsName = stringify(DC_Set);
		int A;

		DC_Set(int A = 0) : DataContainer(clsName), A(A) {};
	};

	struct StateMachineTestInterface
	{
		StateMachineTestInterface()
		{
			setData = [](DC_Set *pData)
			{};
			flush = []()
			{ return false; };
		}

		std::function<void(DC_Set *pData)> setData;
		std::function<bool()> flush;
	};

	struct DC_Start : public DataContainer
	{
		static constexpr const char* clsName = stringify(DC_Start);
		StateMachineTestInterface *testInterface;
		DC_Start(StateMachineTestInterface *testInterface = nullptr)
				: DataContainer(clsName)
				, testInterface(testInterface){};
	};

	struct DC_Stop : public DataContainer
	{
		static constexpr const char* clsName = stringify(DC_Stop);
		DC_Stop() : DataContainer(clsName) {};
	};

	struct DC_Send : public DataContainer
	{
		static constexpr const char* clsName = stringify(DC_Send);
		DC_Send() : DataContainer(clsName) {};
	};

	struct DC_Crash : public DataContainer
	{
		static constexpr const char* clsName = stringify(DC_Crash);
		DC_Crash() : DataContainer(clsName) {};
	};

	struct DC_Common1 : public DataContainer
	{
		static constexpr const char* clsName = stringify(DC_Common1);
		DC_Common1() : DataContainer(clsName) {};
	};

	struct DC_Common2 : public DataContainer
	{
		static constexpr const char* clsName = stringify(DC_Common2);
		DC_Common2() : DataContainer(clsName) {};
	};


	class StateMachineTester : public StateMachine
	{
		using thisCls = StateMachineTester;
	private:
		enum eStates : tState
		{
			ST_NULL = 0,
			ST_STARTED,
			ST_CRASHED,
			ST_MAX_VAL,
		};
	public:
		StateMachineTester() : StateMachine(ST_NULL)
		{
			StateMapCreator smc(ST_MAX_VAL);

			smc.useAction(DCID0(DC_Start));
			smc.add(ST_NULL, ST_STARTED, SMA(act_start));
			smc.add(ST_STARTED);
			smc.add(ST_CRASHED, ST_NULL, SMA(act_clearAndRestart));

			smc.useAction(DCID0(DC_Stop));
			smc.add(ST_NULL);
			smc.add(ST_STARTED, ST_NULL, SMA(act_stop));
			smc.add(ST_CRASHED, ST_NULL, SMA(act_clear));

			smc.useAction(DCID0(DC_Set));
			smc.add(ST_NULL);
			smc.add(ST_STARTED, ST_STARTED, SMA(act_set));
			smc.add(ST_CRASHED);

			smc.useAction(DCID0(DC_Send));
			smc.add(ST_NULL);
			smc.add(ST_STARTED, ST_STARTED, SMA(act_send));
			smc.add(ST_CRASHED);

			smc.useAction(DCID0(DC_Crash));
			smc.add(ST_NULL);
			smc.add(ST_STARTED, ST_STARTED, SMA(act_crash));
			smc.add(ST_CRASHED);

			smc.useAction({DCID0(DC_Common1), DCID0(DC_Common2)});
			smc.add(ST_NULL);
			smc.add(ST_STARTED, ST_STARTED, SMA(act_common));
			smc.add(ST_CRASHED);
			initStateMap(smc);

			thisInterface.setData = [this](DC_Set *pData)
				{ this->doAction(pData); };
		};

		~StateMachineTester()
		{
			delete data;
		}

		StateMachineTestInterface *getInterface()
		{
			return &thisInterface;
		}

		int getData()
		{
			if (data == nullptr)
				return -1;
			return *data;
		}

		void doSet(int A)
		{
			doAction(new DC_Set(A));
		}

		const tState getState()
		{
			return currentState;
		}

	private:
		void freeData()
		{
			delete data;
			data = nullptr;
		}

		void act_start(DC_Start *pData)
		{
			if (pData != nullptr)
				outInterface = pData->testInterface;
			data = new int();
			*data = 10;
		}

		void act_clear(DCP)
		{
			freeData();
		}

		void act_clearAndRestart(DCP)
		{
			freeData();
			internalAction(ActionCore{SMA(act_start), ST_STARTED});
		}

		void act_stop(DCP)
		{
			freeData();
		}

		void act_set(DC_Set *pData)
		{
			*data = pData->A;
		}

		void act_send(DCP)
		{
			outInterface->setData(new DC_Set(*data));
		}

		void act_crash(DCP)
		{
			internalAction(ActionCore{SMA(act_crash_process), ST_CRASHED});
		}

		void act_crash_process(DCP)
		{
			*data = 100;
		}

		void act_common(DataContainer *pDC)
		{
			if (pDC->getType() == DCID(DC_Common1))
				*data = 301;
			if (pDC->getType() == DCID(DC_Common2))
				*data = 302;
		}

		StateMachineTestInterface thisInterface;
		StateMachineTestInterface *outInterface;
		int *data = nullptr;
	};

}

#endif //PAIDAGOGOS_STATE_MACHINE_TEST_H
