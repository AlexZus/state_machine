//
// Created by alex on 26.06.18.
//

#include <boost/test/unit_test.hpp>

#include "tests/state_machine_test.hpp"

BOOST_AUTO_TEST_SUITE(utest_state_machine)

	BOOST_AUTO_TEST_CASE( state_machine_testing )
	{
		using namespace StateMachineTest;
		StateMachineTester A;
		StateMachineTester B;
		BOOST_CHECK_MESSAGE(A.getState() == 0, "A state 0");
		BOOST_CHECK_MESSAGE(B.getState() == 0, "B state 0");
		A.doAction(new DC_Start(B.getInterface()));
		BOOST_CHECK_MESSAGE(A.getState() == 1, "A state 1");
		B.doAction(new DC_Start(A.getInterface()));
		BOOST_CHECK_MESSAGE(B.getState() == 1, "B state 1");
		BOOST_CHECK_MESSAGE(A.getData() == 10, "A 10");
		BOOST_CHECK_MESSAGE(B.getData() == 10, "B 10");

		A.doAction(new DC_Common1());
		BOOST_CHECK_MESSAGE(A.getData() == 301, "DC_Common1");
		A.doAction(new DC_Common2());
		BOOST_CHECK_MESSAGE(A.getData() == 302, "DC_Common2");

		A.doSet(7);
		BOOST_CHECK_MESSAGE(A.getState() == 1, "A state 1");
		BOOST_CHECK_MESSAGE(A.getData() == 7, "A 7");
		A.doAction(DCID(DC_Send));
		BOOST_CHECK_MESSAGE(A.getState() == 1, "A state 1");
		BOOST_CHECK_MESSAGE(B.getData() == 7, "B 7");
		A.doAction(DCID(DC_Stop));
		BOOST_CHECK_MESSAGE(A.getState() == 0, "A state 0");
		BOOST_CHECK_MESSAGE(A.getData() == -1, "A -1");
		B.doAction(DCID(DC_Crash));
		BOOST_CHECK_MESSAGE(B.getState() == 2, "B state 2");
		BOOST_CHECK_MESSAGE(B.getData() == 100, "B 100");
		DC_Start *pDC_Start = new DC_Start();
		pDC_Start->setOwner(this);
		B.doAction(pDC_Start);
		delete pDC_Start;
		BOOST_CHECK_MESSAGE(B.getState() == 1, "B state 1");
		BOOST_CHECK_MESSAGE(B.getData() == 10, "B 100");

	}

BOOST_AUTO_TEST_SUITE_END()