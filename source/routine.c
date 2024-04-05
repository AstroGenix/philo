/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dberehov <dberehov@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/20 11:55:02 by dberehov          #+#    #+#             */
/*   Updated: 2024/04/02 16:40:58 by dberehov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philosophers.h"

/**
 * Makes the philosopher nap for a specified time.
 * The function waits until the specified time has passed.
 * 
 * @param time The time to nap in milliseconds.
 * @param val The table struct.
 */
static void	nap(long long time, t_table *val)
{
	long long	timestamp;

	timestamp = get_current_time();
	while (true)
	{
		if (time_diff(timestamp, get_current_time()) >= time)
			break ;
		usleep(10);
	}
}

/**
 * Represents a philosopher grabbing a fork.
 * The philosopher tries to take the left and right fork.
 * The order depends on the philosopher's ID to prevent deadlock.
 * A message is logged after successfully taking a fork.
 * 
 * @param me The philosopher struct.
 */
static void grab_fork(t_philo *me)
{
	t_table	*val;

	val = me->table;
	if (me->id % 2 == 0)
	{
		pthread_mutex_lock(&(val->fork[me->l_fork]));
		monitor(me, "has taken a fork");
		pthread_mutex_lock(&(val->fork[me->r_fork]));
		monitor(me, "has taken a fork");
	}
	else
	{
		pthread_mutex_lock(&(val->fork[me->r_fork]));
		monitor(me, "has taken a fork");
		pthread_mutex_lock(&(val->fork[me->l_fork]));
		monitor(me, "has taken a fork");
	}
}

/**
 * Represents a philosopher's eating action.
 * The philosopher takes forks, eats, then drops forks.
 * 
 * @param philo The philosopher struct.
 */
static void	eat(t_philo *me)
{
	t_table	*val;

	val = me->table;
	grab_fork(me);
	pthread_mutex_lock(&(me->table->guilty_spark));
	monitor(me, "is eating");
	(me->meal_count)++;
	me->last_meal_time = get_current_time(); // update last_meal_time after eating
	pthread_mutex_unlock(&(me->table->guilty_spark));
	nap(val->time_to_eat, val);
	if (me->id % 2 == 0)
	{
		pthread_mutex_unlock(&(val->fork[me->r_fork]));
		pthread_mutex_unlock(&(val->fork[me->l_fork]));
	}
	else
	{
		pthread_mutex_unlock(&(val->fork[me->l_fork]));
		pthread_mutex_unlock(&(val->fork[me->r_fork]));
	}
}

/**
 * Checks if the simulation should end.
 * If all philosophers have eaten enough, it ends the simulation.
 * 
 * @param val The table struct.
 * @param philo Array of philosopher structs.
 */
static bool	check_end_clause(t_table *val, t_philo *me)
{
	pthread_mutex_lock(&(val->guilty_spark));
	if (val->all_have_eaten == true || val->sim_end == true)
	{
   		pthread_mutex_unlock(&(val->guilty_spark));
		return (true);
	}
	pthread_mutex_unlock(&(val->guilty_spark));
	return (false);
}

/**
 * Represents a philosopher's routine.
 * If only one philosopher, they take a fork and end.
 * If philosopher's ID is odd, they wait briefly.
 * Then they loop to eat, sleep, and think until simulation ends.
 * 
 * @param philo The philosopher struct.
 * @return NULL always.
 */
void	*routine(void *philo)
{
	t_philo	*me;
	t_table	*val;

	me = (t_philo *)philo;
	val = me->table;
	if (me->table->total_philo == 1)
	{
		monitor(me, "has taken a fork");
		return (NULL);
	}
	if (me->id % 2)
		usleep(10000);
	while (true)
	{
		if (check_end_clause(val, me))
			break ;
		eat(me);
		monitor(me, "is sleeping");
		nap(val->time_to_sleep, val);
		monitor(me, "is thinking");
	}
	return (NULL);
}
