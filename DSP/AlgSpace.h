/*
  ==============================================================================

    AlgSpace.h
    Created: 6 Sep 2025 3:21:31pm
    Author:  ryanb

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Operator.h"
class AlgSpace
{
public:

	AlgSpace() = default;
	~AlgSpace() = default;

    /*
	setAlgorithm will set modulation routing, assign carriers, and set feedback 
	for all 32 algorithms
	*/
	void setAlgorithm(std::vector<Operator>& op, int algIndex) {
		if (algIndex < 0 || algIndex >= algTable.size()) {
			jassertfalse;
			return;
		}
		// clear routings
		for (auto& opRef : op)
			opRef.resetRouting();

		const auto& alg = algTable[algIndex];
		// map is one-based indexing, hence - 1
		for (const auto& routing : alg.routings) {
			if (routing.modulated > 0 && routing.modulator > 0)
				op[routing.modulated - 1].addModOperator(&op[routing.modulator - 1]);
		}
		for (int c : alg.carriers)
			op[c - 1].setCarrier(true);
		if (alg.feedbackOperator != -1)
			op[alg.feedbackOperator - 1].setFeedback(true);
	}
private:
	struct algRouting {
		int modulated;
		int modulator;
	};
	struct algDescription {
		std::vector<algRouting> routings;
		std::vector<int> carriers;
		int feedbackOperator; // -1 means no feedback opereator
	};
	const std::vector<algDescription> algTable = {
		/*
		Format:
		{ { {modulated, modulator}, ... }, // routings
			{carriers}, // carrier indeces
			feedbackOperator } // feedback operator
			// These are ONE indexed to be 1-1 with alg graphs
		*/
		// Alg 1
		{
			{{1, 2}, {3, 4}, {4, 5}, {5, 6}},
			{1, 3},
			6
		},
		// Alg 2
		{
			{{1, 2}, {3, 4}, {4, 5}, {5, 6}},
			{1, 3},
			2
		},
		// Alg 3
		{
			{{1, 2}, {2, 3}, {4, 5}, {5, 6}},
			{1, 4},
			6
		},
		// Alg 4
		{
			{{1, 2}, {2, 3}, {4, 5}, {5, 6}, {6, 4}},
			{ 1, 4 },
			-1
		},
		// Alg 5
		{
			{ { 1, 2 }, {3, 4}, {5, 6}},
			{1, 3, 5},
			6
		},
		// Alg 6
		{
			{ { 1, 2 }, {3, 4}, {5, 6}, { 6, 5 }},
			{1, 3, 5},
			-1
		},
		// Alg 7
		{
			{{1 ,2}, {3, 4}, {3, 5}, {5, 6}},
			{1, 3},
			6
		},
		// Alg 8
		{
			{{1 ,2}, {3, 4}, {3, 5}, {5, 6}},
			{1, 3},
			4
		},
		// Alg 9
		{
			{{1 ,2}, {3, 4}, {3, 5}, {5, 6}},
			{1, 3},
			2
		},
		// Alg 10
		{
			{{1, 2}, {2, 3}, {4, 5}, {4, 6}},
			{1, 4},
			3
		},
		// Alg 11
		{
			{{1, 2}, {2, 3}, {4, 5}, {4, 6}},
			{1, 4},
			6
		},
		// Alg 12
		{
			{{1, 2}, {3, 4}, {3, 5}, {3,6}},
			{1, 3},
			2
		},
		// Alg 13
		{
			{ { 1, 2 }, { 3, 4 }, { 3, 5 }, { 3,6 }},
			{ 1, 3 },
			6
		},
		// Alg 14
		{
			{{1, 2}, {3, 4},{4, 5}, {4, 6}},
			{1, 3},
			6
		},
		// Alg 15
		{
			{{1, 2}, {3, 4},{4, 5}, {4, 6}},
			{1, 3},
			2
		},
		// Alg 16
		{
			{{1, 2}, {1, 3}, {1, 5}, {3, 4}, {5, 6}},
			{1},
			6
		},
		// Alg 17
		{
			{{1, 2}, {1, 3}, {1, 5}, {3, 4}, {5, 6}},
			{1},
			2
		},
		// Alg 18 (TODO: gui is wrong)
		{
			{{1, 2}, {1, 3}, {1, 4}, {4, 5}, {5, 6}},
			{1},
			3
		},
		// Alg 19
		{
			{{1, 2}, {2, 3}, {4, 6}, {5, 6}},
			{1, 4 ,5},
			6
		},
		// Alg 20
		{
			{{1, 3}, {2, 3}, {4, 5}, {4, 6}},
			{1, 2, 4},
			3
		},
		// Alg 21
		{
			{{1, 3}, {2, 3}, {4, 6}, {5, 6}},
			{1, 2, 4, 5},
			3
		},
		// Alg 22
		{
			{{1, 2}, {3, 6}, {4, 6}, {5, 6}},
			{1, 3, 4, 5},
			6
		},
		// Alg 23
		{
			{{2, 3}, {4, 6}, {5, 6}},
			{1, 2, 4, 5},
			6
		},
		// Alg 24
		{
			{{3, 6}, {4, 6}, {5, 6}},
			{1, 2, 3, 4, 5},
			6
		},
		// Alg 25
		{
			{{4, 6}, {5, 6}},
			{1, 2, 3, 4, 5},
			6
		},
		// Alg 26
		{
			{{2, 3}, {4, 5}, {4, 6}},
			{1, 2, 4},
			6
		},
		// Alg 27
		{
			{{2, 3}, {4, 5}, {4, 6}},
			{1, 2, 4},
			3
		},
		// Alg 28
		{
			{{1, 2}, {3, 4}, {4, 5}},
			{1, 3, 6},
			5
		},
		// Alg 29 
		{
			{{3, 4}, {5, 6}},
			{1, 2, 3, 5},
			6
		},
		// Alg 30
		{
			{{3, 4}, {4, 5}},
			{1, 2, 3, 6},
			5
		},
		// Alg 31
		{
			{{5, 6}},
			{1, 2, 3, 4, 5},
			6
		},
		// Alg 32
		{
			{{-1, -1}},
			{1, 2, 3, 4, 5, 6},
			6
		},
	};
};