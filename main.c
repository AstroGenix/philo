/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dberehov <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 09:31:34 by dberehov          #+#    #+#             */
/*   Updated: 2024/01/17 09:31:35 by dberehov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/philosophers.h"

int	main(int argn, char *args[])
{
	t_table			table;
	t_philo			*philo;
	pthread_mutex_t	*fork;

	if (argn != 5 && argn != 6)
		err_exit("Incorrect number of arguments");
	if (arg_check(argn, args) == true)
		err_exit("One or more arguments are not numbers");
	if (init_values(&table, args) == true)
		err_exit("One or more arguments have invalid values");
	fork = init_fork(&table);
	philo = init_philo(&table, fork);
	create_join_threads(&table, fork, philo);
	cleanup(&table, fork, philo);
	return (0);
}

void	cleanup(t_table *val, pthread_mutex_t *fork, t_philo *philo)
{
	int	i;

	i = 0;
	pthread_mutex_destroy(&val->guilty_spark);
	if (fork)
	{
		while (i < val->philo_num)
		{
			pthread_mutex_destroy(&fork[i]);
			i++;
		}
		free(fork);
	}
	if (philo)
		free(philo);
}

//Create then join the threads
void	create_join_threads(t_table *val, pthread_mutex_t *fork, t_philo *philo)
{
	int	i;

	i = -1;
	while (++i < val->philo_num)
	{
		philo[i].start_time = cur_time();
		if (pthread_create(&philo[i].thread_id, NULL, routine, (void *)&philo[i]) != 0)
			err_exit("Could not create philo thread");
	}
	catch_end_clause(val, philo);
	i = -1;
	while (++i < val->philo_num)
	{	
		if (pthread_join(philo[i].thread_id, NULL) != 0)
			err_exit("Could not join philo thread");
	}
	return ;
}
