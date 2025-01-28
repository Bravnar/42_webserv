/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   colors.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rude-jes <rude-jes@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 16:26:51 by rude-jes          #+#    #+#             */
/*   Updated: 2024/10/31 16:01:46 by rude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLORS_H
# define COLORS_H

// Standard 16 Colors
# define C_BLACK       "\001\033[30m\002"
# define C_RED         "\001\033[31m\002"
# define C_GREEN       "\001\033[32m\002"
# define C_YELLOW      "\001\033[33m\002"
# define C_BLUE        "\001\033[34m\002"
# define C_MAGENTA     "\001\033[35m\002"
# define C_CYAN        "\001\033[36m\002"
# define C_WHITE       "\001\033[37m\002"

# define C_B_BLACK      "\001\033[48;5;0m\002"
# define C_B_RED        "\001\033[48;5;1m\002"
# define C_B_GREEN      "\001\033[48;5;2m\002"
# define C_B_YELLOW     "\001\033[48;5;3m\002"
# define C_B_BLUE       "\001\033[48;5;4m\002"
# define C_B_MAGENTA    "\001\033[48;5;5m\002"
# define C_B_CYAN       "\001\033[48;5;6m\002"
# define C_B_WHITE      "\001\033[48;5;7m\002"

# define C_RESET       "\001\033[0m\002"

// Additional named colors (using 256-color mode)
# define C_ORANGE      "\001\033[38;5;214m\002"
# define C_PINK        "\001\033[38;5;206m\002"
# define C_PURPLE      "\001\033[38;5;93m\002"
# define C_BROWN       "\001\033[38;5;94m\002"
# define C_GOLD        "\001\033[38;5;220m\002"
# define C_SILVER      "\001\033[38;5;250m\002"
# define C_GRAY        "\001\033[38;5;244m\002"
# define C_DARKGRAY    "\001\033[38;5;240m\002"
# define C_LIGHTBLUE   "\001\033[38;5;39m\002"
# define C_LIGHTGREEN  "\001\033[38;5;83m\002"
# define C_LIGHTCYAN   "\001\033[38;5;87m\002"
# define C_LIGHTRED    "\001\033[38;5;203m\002"
# define C_LIGHTYELLOW "\001\033[38;5;227m\002"
# define C_LIGHTMAGENTA "\001\033[38;5;207m\002"

// Background colors
# define C_B_ORANGE    "\001\033[48;5;214m\002"
# define C_B_PINK      "\001\033[48;5;206m\002"
# define C_B_PURPLE    "\001\033[48;5;93m\002"
# define C_B_BROWN     "\001\033[48;5;94m\002"
# define C_B_GOLD      "\001\033[48;5;220m\002"
# define C_B_SILVER    "\001\033[48;5;250m\002"
# define C_B_GRAY      "\001\033[48;5;244m\002"

// Text effects
# define C_BOLD        "\001\033[1m\002"
# define C_DIM         "\001\033[2m\002"
# define C_UNDERLINE   "\001\033[4m\002"
# define C_BLINK       "\001\033[5m\002"
# define C_REVERSE     "\001\033[7m\002"
# define C_INVISIBLE   "\001\033[8m\002"
# define C_STRIKETHROUGH "\001\033[9m\002"
# define C_CLEAR       "\001\033[2J\001\033[1;1H"

#endif