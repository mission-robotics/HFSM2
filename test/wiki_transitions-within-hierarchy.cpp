// HFSM2 (hierarchical state machine for games and interactive applications)
// Created by Andrew Gresyk

#define HFSM2_ENABLE_UTILITY_THEORY
#include <hfsm2/machine.hpp>

#include <doctest/doctest.h>

////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Wiki.Transitions within Hierarchy.Transitions into States") {
    using M = hfsm2::Machine;

    using FSM = M::PeerRoot<
                    struct Origin,
                    struct Destination
                >;

    struct Origin      : FSM::State {};
    struct Destination : FSM::State {};

    FSM::Instance fsm;
    REQUIRE(fsm.isActive<Origin>());

    fsm.immediateChangeTo<Destination>();
    REQUIRE(fsm.isActive<Destination>());
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Wiki.Transitions within Hierarchy.Transitions into Regions") {
    using M = hfsm2::Machine;

    using FSM = M::PeerRoot<
                    struct Origin,
                    M::Composite<struct Destination,
                        struct SubState1,
                        struct SubState2
                    >
                >;

    struct Origin      : FSM::State {};
    struct Destination : FSM::State {};
    struct SubState1   : FSM::State {};
    struct SubState2   : FSM::State {};

    FSM::Instance fsm;
    REQUIRE(fsm.isActive<Origin>());

    fsm.immediateChangeTo<Destination>();
    REQUIRE(fsm.isActive<Destination>());
    REQUIRE(fsm.isActive<SubState1>());
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Wiki.Transitions within Hierarchy.External Transition Interface") {
    using M = hfsm2::Machine;

    using FSM = M::PeerRoot<
                    struct Origin,
                    struct Destination
                >;

    struct Origin       : FSM::State {};
    struct Destination : FSM::State {};

    FSM::Instance fsm;
    REQUIRE(fsm.isActive<Origin>());

    fsm.immediateChangeTo<Destination>(); // external transition
    REQUIRE(fsm.isActive<Destination>());
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Wiki.Transitions within Hierarchy.Internal Transition Interface") {
    using M = hfsm2::Machine;

    using FSM = M::PeerRoot<
                    struct Origin,
                    struct Destination
                >;

    struct Origin
        : FSM::State
    {
        void update(FullControl& control) noexcept {
            control.changeTo<Destination>(); // internal transition
        }
    };

    struct Destination : FSM::State {};

    FSM::Instance fsm;
    REQUIRE(fsm.isActive<Origin>());

    fsm.update();
    REQUIRE(fsm.isActive<Destination>());
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Wiki.Transitions within Hierarchy.'Restart' Transition") {
    using M = hfsm2::Machine;

    using FSM = M::PeerRoot<
                    struct State,
                    M::Composite<struct Region,
                        struct Initial,
                        struct Secondary
                    >
                >;

    struct State     : FSM::State {};
    struct Region    : FSM::State {};
    struct Initial   : FSM::State {};
    struct Secondary : FSM::State {};

    FSM::Instance fsm;
    REQUIRE(fsm.isActive<State>());

    fsm.immediateChangeTo<Region>();
    REQUIRE(fsm.isActive<Initial>());
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Wiki.Transitions within Hierarchy.'Resume' Transition") {
    using M = hfsm2::Machine;

    using FSM = M::PeerRoot<
                    struct State,
                    M::Composite<struct Region,
                        struct Initial,
                        struct Secondary
                    >
                >;

    struct State     : FSM::State {};
    struct Region    : FSM::State {};
    struct Initial   : FSM::State {};
    struct Secondary : FSM::State {};

    FSM::Instance fsm;
    REQUIRE(fsm.isActive<State>());

    fsm.immediateChangeTo<Secondary>();
    REQUIRE(fsm.isActive<Secondary>());

    fsm.immediateChangeTo<State>();
    REQUIRE(fsm.isActive<State>());

    fsm.immediateResume<Region>();
    REQUIRE(fsm.isActive<Secondary>());

    fsm.immediateRestart<Region>();
    REQUIRE(fsm.isActive<Initial>());
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Wiki.Transitions within Hierarchy.'Utilize' Transition") {
    using M = hfsm2::Machine;

    using FSM = M::PeerRoot<
                    struct State,
                    M::Composite<struct Region,
                        struct LowRated,
                        struct HighRated
                    >
                >;

    struct State     : FSM::State {};
    struct Region    : FSM::State {};

    struct LowRated
        : FSM::State
    {
        float utility(const Control&) noexcept { return 0.5f; }
    };

    struct HighRated
        : FSM::State
    {
        float utility(const Control&) noexcept { return 2.0f; }
    };

    FSM::Instance fsm;
    REQUIRE(fsm.isActive<State>());

    fsm.immediateUtilize<Region>();
    REQUIRE(fsm.isActive<HighRated>());
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Wiki.Transitions within Hierarchy.'Randomize' Transition") {
    using M = hfsm2::Machine;

    using FSM = M::PeerRoot<
                    struct State,
                    M::Composite<struct Region,
                        struct FilteredOut,
                        struct LowRated,
                        struct HighRated
                    >
                >;

    struct State     : FSM::State {};
    struct Region    : FSM::State {};

    struct FilteredOut
        : FSM::State
    {
        int8_t rank(const Control&) noexcept { return 0; } // filter out using low rank

        float utility(const Control&) noexcept { return 0.5f; }
    };

    struct LowRated
        : FSM::State
    {
        int8_t rank(const Control&) noexcept { return 1; }

        float utility(const Control&) noexcept { return 0.5f; }
    };

    struct HighRated
        : FSM::State
    {
        int8_t rank(const Control&) noexcept { return 1; }

        float utility(const Control&) noexcept { return 2.0f; }
    };

    FSM::Instance fsm;
    REQUIRE(fsm.isActive<State>());

    fsm.immediateRandomize<Region>();
    REQUIRE(fsm.isActive<HighRated>()); // note, it could be LowRated if the PRNG is seeded differently
}

////////////////////////////////////////////////////////////////////////////////
