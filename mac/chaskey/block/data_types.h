/*
 *
 * Katholieke Universiteit Leuven
 * Computer Security and Industrial Cryptography (COSIC)
 *
 * FELICS - Fair Evaluation of Lightweight Cryptographic Systems
 *
 * Copyright (C) 2015 Katholieke Universiteit Leuven
 *
 * Written in 2015 by Nicky Mouha <nicky.mouha@esat.kuleuven.be>
 *
 * This file is part of FELICS.
 *
 * FELICS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * FELICS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 *
 * University of Luxembourg
 * Laboratory of Algorithmics, Cryptology and Security (LACS)
 *
 * FELICS - Fair Evaluation of Lightweight Cryptographic Systems
 *
 * Copyright (C) 2015 University of Luxembourg
 *
 * Written in 2015 by Daniel Dinu <dumitru-daniel.dinu@uni.lu>
 *
 * This file is part of FELICS.
 *
 * FELICS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * FELICS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include "cipher.h"


/*
 *
 * Implementation data types
 *
 */


#if defined(PC) /* PC */
	/* Architecture = PC ; Scenario = 0 (cipher operation) */
	#if defined(SCENARIO) && (SCENARIO_0 == SCENARIO)
	#endif
	/* Architecture = PC ; Scenario = 1 */
	#if defined(SCENARIO) && (SCENARIO_1 == SCENARIO)
	#endif
	/* Architecture = PC ; Scenario = 2 */
	#if defined(SCENARIO) && (SCENARIO_2 == SCENARIO)
	#endif
#endif /* PC */



#if defined(AVR) /* AVR */
	/* Architecture = AVR ; Scenario = 0 (cipher operation) */
	#if defined(SCENARIO) && (SCENARIO_0 == SCENARIO)
	#endif
	/* Architecture = AVR ; Scenario = 1 */
	#if defined(SCENARIO) && (SCENARIO_1 == SCENARIO)
	#endif
	/* Architecture = AVR ; Scenario = 2 */
	#if defined(SCENARIO) && (SCENARIO_2 == SCENARIO)
	#endif
#endif /* AVR */



#if defined(MSP) /* MSP */
	/* Architecture = MSP ; Scenario = 0 (cipher operation) */
	#if defined(SCENARIO) && (SCENARIO_0 == SCENARIO)
	#endif
	/* Architecture = MSP ; Scenario = 1 */
	#if defined(SCENARIO) && (SCENARIO_1 == SCENARIO)
	#endif
	/* Architecture = MSP ; Scenario = 2 */
	#if defined(SCENARIO) && (SCENARIO_2 == SCENARIO)
	#endif
#endif /* MSP */



#if defined(ARM) /* ARM */
	/* Architecture = ARM ; Scenario = 0 (cipher operation) */
	#if defined(SCENARIO) && (SCENARIO_0 == SCENARIO)
	#endif
	/* Architecture = ARM ; Scenario = 1 */
	#if defined(SCENARIO) && (SCENARIO_1 == SCENARIO)
	#endif
	/* Architecture = ARM ; Scenario = 2 */
	#if defined(SCENARIO) && (SCENARIO_2 == SCENARIO)
	#endif
#endif /* ARM */


#endif /* DATA_TYPES_H */
