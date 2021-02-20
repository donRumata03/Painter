//
// Created by Vova on 20.02.2021.
//

#pragma once

#include "common_operations/stroke_limits.h"

#include "util.h"



/**
 * Callable object for mutation, which automatically computes current sigmas and mutation gene number «on-fly» by «mutation amount» (for example, in annealing)
 */
class SingleGenomeMutator
{
	stroke_limit_descriptor limits{};

	/// Mutation intensiveness:
	std::vector<double> max_mutation_sigmas;
	double max_target_mutation_gene_amount = 0;
	double move_probability = -1;

	normalizer normaa; // See https://www.youtube.com/watch?v=duvlWEJJmU0
	mutator m_mutator {};


	SingleGenomeMutator() : normaa(10'000) {}
	SingleGenomeMutator(stroke_limit_descriptor limits,	std::vector<double> max_mutation_sigmas, double max_target_mutation_gene_amount, double move_probability)
								: limits(limits), max_mutation_sigmas(std::move(max_mutation_sigmas)),
								max_target_mutation_gene_amount(max_target_mutation_gene_amount), move_probability(move_probability), normaa(10'000)
	{
		m_mutator = mutator(limits, move_probability);

		// normaa.add_samples(10'000);
	}

	std::vector<double> operator()(const std::vector<double>& stroke_data_buffer, double amount) const
	{
		/// Amount: [0; 1]
		/// Here we need to generate
		/// sigmas (std::vector<double>), target_gene_number (double)

		double adjusted_amount = transfer_range(amount, { 0., 1. }, { 0.1, 1. });

		size_t genome_elements = stroke_data_buffer.size();
		assert(genome_elements == max_mutation_sigmas.size());
		size_t stroke_number = genome_elements / (sizeof(stroke) / sizeof(double));

		std::vector<double> stroke_sigmas(max_mutation_sigmas.begin(), max_mutation_sigmas.end());
		// Decrease by multiplying by adjusted_amount:
		std::transform(stroke_sigmas.begin(), stroke_sigmas.end(), stroke_sigmas.begin(), [&adjusted_amount](double value){ return value * adjusted_amount; });

		double target_gene_number = max_target_mutation_gene_amount * adjusted_amount;

		/// The actual mutation:
		std::vector<double> result = stroke_data_buffer;
		m_mutator(result, stroke_sigmas, target_gene_number, normaa);

		return result;
	}


};


