#include "pch.h"
#include "Random.h"

Random::Random() : rng(std::random_device{}()) 
{}

bool Random::getBool() {
    std::bernoulli_distribution dist(0.5);
    return dist(rng);
}
