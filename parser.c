#include "parser.h"
int flag=0;
void print_bits(long long value) {
    for (int i = sizeof(value) * 8 - 1; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
    }
    printf("\n");
    			if(flag){

				/*printf("CAR %lld\n\n", CCONS_CAR);
				printf("PAR %lld\n\n", CPAR_CIE);
				printf("LOOK %lld\n\n", lookahead());*/

				printf("CAR ");print_bits(CCONS_CAR);
				printf("PAR ");print_bits(CPAR_CIE);
				printf("LOO ");print_bits(lookahead());
				printf("\n\n    LEXEMA: %s   \n\n", sbol->lexema);
	}
}

// Funcion de test
void test(set cjto1, set cjto2, int n){
	// Controla si el token esta en el conjunto de lo esperado
	if (!lookahead_in(cjto1))
	{
		error_handler(n);
		// Combina cjto1 y cjto2 en cjtoAux
		set cjtoAux = cjto1 | cjto2;
		while (!lookahead_in(cjtoAux)){
			scanner();
		}
	}
}

int main(int argc, char *argv[])
{
	init_parser(argc, argv);

	unidad_traduccion(CEOF);

	match(CEOF, 9);

	last_call=1;

	error_handler(COD_IMP_ERRORES);

	return 0;
}


/********* funciones del parser ***********/

void unidad_traduccion(set folset)
{
	test((CVOID | CCHAR | CINT | CFLOAT), folset, 40);
	while(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		declaraciones(folset | (CVOID | CCHAR | CINT | CFLOAT));
}


void declaraciones(set folset)
{
	especificador_tipo(folset | CIDENT | CPAR_ABR | CIGUAL | CCOR_ABR | CCOMA | CPYCOMA);

	match(CIDENT, 17);

	especificador_declaracion(folset);
}


void especificador_tipo(set folset)
{
	test((CVOID | CCHAR | CINT | CFLOAT), folset, 41);
	switch(lookahead())
	{
		case CVOID:
			scanner();
			break;

		case CCHAR:
			scanner();
			break;

		case CINT:
			scanner();
			break;

		case CFLOAT:
			scanner();
			break;

		default:
			error_handler(18);
	}
	test(folset, NADA, 42);
}


void especificador_declaracion(set folset)
{
	test((CPAR_ABR | CASIGNAC | CCOR_ABR | CCOMA | CPYCOMA), folset, 43);
	switch(lookahead())
	{
		case CPAR_ABR:
			definicion_funcion(folset);
			break;

		case CASIGNAC:
		case CCOR_ABR:
		case CCOMA:
		case CPYCOMA:
			declaracion_variable(folset);
			break;

		default:
			error_handler(19);
	}
}


void definicion_funcion(set folset)
{
	match(CPAR_ABR, 20);
	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		lista_declaraciones_param(folset | (CPAR_CIE | CLLA_ABR));

	match(CPAR_CIE, 21);

	proposicion_compuesta(folset);
}


void lista_declaraciones_param(set folset)
{
	declaracion_parametro(folset | CCOMA | (CVOID | CCHAR | CINT | CFLOAT));

	while(lookahead_in(CCOMA | (CVOID | CCHAR | CINT | CFLOAT)))
	{
		match(CCOMA, 64);
		declaracion_parametro(folset | CCOMA | (CVOID | CCHAR | CINT | CFLOAT));
	}
}


void declaracion_parametro(set folset)
{
	especificador_tipo(folset | CAMPER | CIDENT | CCOR_ABR| CCOR_CIE);

	if(lookahead_in(CAMPER))
		scanner();

	match(CIDENT, 17);

	if(lookahead_in(CCOR_ABR | CCOR_CIE))
	{
		match(CCOR_ABR, 35);
		match(CCOR_CIE, 22);
	}
	test(folset, NADA, 45);
}


void lista_declaraciones_init(set folset)
{
	test(CIDENT, folset | (CCOMA | CIGUAL | CCOR_ABR), 46);

	match(CIDENT, 17);

	declarador_init(folset | CCOMA | CIDENT | CIGUAL | CCOR_ABR);

	while(lookahead_in(CCOMA | (CIDENT | CIGUAL | CCOR_ABR)))
	{
		match(CCOMA, 64);
		match(CIDENT, 17);
		declarador_init(folset | CCOMA | CIDENT | CIGUAL | CCOR_ABR);
	}
}


void declaracion_variable(set folset)
{
	declarador_init(folset | CCOMA | CPYCOMA | CIDENT);

	if(lookahead_in(CCOMA | CIDENT))
	{
		match(CCOMA, 64);
		lista_declaraciones_init(folset | CPYCOMA);
	}

	match(CPYCOMA, 23);

	test(folset, NADA, 51);
}


void declarador_init(set folset)
{
	test(folset | CASIGNAC | CCOR_ABR, CLLA_ABR | CLLA_CIE, 47);
	switch(lookahead())
	{
		case CASIGNAC:
			scanner();
			constante(folset);
			break;

		case CCOR_ABR:
		// Punstos de reconfiguracion
		case CLLA_ABR:
		case CLLA_CIE:
			match(CCOR_ABR, 35);

			if(lookahead_in(CCONS_ENT))
				constante(folset | CCOR_CIE | CASIGNAC | CLLA_ABR | CLLA_CIE |
					(CCONS_ENT | CCONS_FLO | CCONS_CAR));

			match(CCOR_CIE, 22);

			if(lookahead_in(CASIGNAC) || lookahead_in( CASIGNAC |
				CLLA_ABR | CLLA_CIE | (CCONS_ENT | CCONS_FLO | CCONS_CAR)))
			{
				match(CASIGNAC, 66);
				match(CLLA_ABR, 24);
				lista_inicializadores(folset | CLLA_CIE);
				match(CLLA_CIE, 25);
			}
			break;
	}
	test(folset, NADA, 48);
}


void lista_inicializadores(set folset)
{
	constante(folset | CCOMA | (CCONS_ENT | CCONS_FLO | CCONS_CAR));

	while(lookahead_in(CCOMA) || lookahead_in(CCOMA | (CCONS_ENT | CCONS_FLO | CCONS_CAR)))
	{
		match(CCOMA, 64);
		constante(folset | CCOMA | (CCONS_ENT | CCONS_FLO | CCONS_CAR));
	}
}


void proposicion_compuesta(set folset)
{
	test(CLLA_ABR, folset | CLLA_CIE | (CVOID | CCHAR | CINT | CFLOAT), 49); //First(list_props) in follow

	match(CLLA_ABR, 24);

	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		lista_declaraciones(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
           CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
           CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN);

	if(lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
						 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
						 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
		lista_proposiciones(folset | CLLA_CIE);

	match(CLLA_CIE, 25);
	test(folset, NADA, 50);
}


void lista_declaraciones(set folset)
{
	declaracion(folset | (CVOID | CCHAR | CINT | CFLOAT));
	while(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		declaracion(folset | (CVOID | CCHAR | CINT | CFLOAT));
}


void declaracion(set folset)
{
	especificador_tipo(folset | CIDENT | CPYCOMA);
	lista_declaraciones_init(folset | CPYCOMA);
	match(CPYCOMA, 23);
	test(folset, NADA, 51);
}


void lista_proposiciones(set folset)
{
	proposicion(folset | (CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
		CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
		CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN));

	while(lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
		proposicion(folset | (CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
			CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
			CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN));
}


void proposicion(set folset)
{
	test(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
		CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
		CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN,
		folset, 52
	);
	switch(lookahead())
	{
		case CLLA_ABR:
			proposicion_compuesta(folset);
			break;

		case CWHILE:
			proposicion_iteracion(folset);
			break;

		case CIF:
			proposicion_seleccion(folset);
			break;

		case CIN:
		case COUT:
			proposicion_e_s(folset);
			break;

		case CMAS:
		case CMENOS:
		case CIDENT:
		case CPAR_ABR:
		case CNEG:
		case CCONS_ENT:
		case CCONS_FLO:
		case CCONS_CAR:
		case CCONS_STR:
		case CPYCOMA:
			proposicion_expresion(folset);
			break;

		case CRETURN:
			proposicion_retorno(folset);
			break;

		default:
			error_handler(26);
	}
}


void proposicion_iteracion(set folset)
{
	match(CWHILE, 27);

	match(CPAR_ABR, 20);

	expresion(folset |(CMAS | CMENOS | CLLA_ABR | CWHILE | CIF | CRETURN | CIN | COUT
		| CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR
		| CCONS_STR | CPYCOMA) | CPAR_CIE);

	match(CPAR_CIE, 21);

	proposicion(folset);
}


void proposicion_seleccion(set folset)
{
	match(CIF, 28);

	match(CPAR_ABR, 20);


	expresion(folset | CPAR_CIE | CELSE |
	    (CMAS | CMENOS | CLLA_ABR | CWHILE | CIF | CRETURN | CIN | COUT
		| CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR
		| CCONS_STR | CPYCOMA)
    );

	match(CPAR_CIE, 21);

	proposicion(folset | CELSE);

	if(lookahead_in(CELSE))
	{
		match(CELSE, 36);
		proposicion(folset);
	}
}


void proposicion_e_s(set folset)
{
	switch(lookahead())
	{
		case CIN:
			scanner();

			match(CSHR, 30);

			variable(folset | CSHR | CPYCOMA | CIDENT);

			while(lookahead_in(CSHR | CIDENT))
			{
				match(CSHR, 30);
				variable(folset | CSHR | CPYCOMA | CIDENT);
			}

			match(CPYCOMA, 23);

			break;

		case COUT:
			scanner();

			match(CSHL, 31);

			expresion(folset | CSHL | CPYCOMA |
			(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR));

			while(lookahead_in(CSHL))
			{
				match(CSHL, 31);
				expresion(folset | CSHL | CPYCOMA | (CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR));
			}

			match(CPYCOMA, 23);

			break;

		default:
			error_handler(29);
	}
	test(folset, NADA, 53);
}

void proposicion_retorno(set folset)
{
	match(CRETURN, 37);

	expresion(folset | CPYCOMA);

	match(CPYCOMA, 23);

	test(folset, NADA, 54);
}


void proposicion_expresion(set folset)
{
	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		expresion(folset | CPYCOMA);

	match(CPYCOMA, 23);

	test(folset, NADA, 55);
}


void expresion(set folset)
{
	expresion_simple(folset | (CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG));

	while(lookahead_in(CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG))
	{
		switch(lookahead())
		{
			case CASIGNAC:
				scanner();
				expresion_simple(folset | (CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG));
				break;

			case CDISTINTO:
			case CIGUAL:
			case CMENOR:
			case CMEIG:
			case CMAYOR:
			case CMAIG:
				scanner();
				expresion_simple(folset | (CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG));
				break;
		}
	}
}

void expresion_simple(set folset)
{
	test(CMAS | CMENOS| (CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR |
		CNEG | CPAR_ABR | CCONS_STR), folset | COR, 56);

	if(lookahead_in(CMAS | CMENOS))
		scanner();

	termino(folset | CMAS | CMENOS | COR | (CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR |
		CNEG | CPAR_ABR | CCONS_STR));

	while(lookahead_in(CMAS | CMENOS | COR) ||
		lookahead_in(CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR |
		CNEG | CPAR_ABR | CCONS_STR))
	{
		if(lookahead_in(CMAS | CMENOS | COR))
			scanner();
		else
			error_handler(65);
		termino(folset | COR | (CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR |
		CNEG | CPAR_ABR | CCONS_STR));
	}
}


void termino(set folset)
{
	factor(folset | (CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR
			| CCONS_STR));
	while(lookahead_in(CMULT | CDIV | CAND | (CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR
			| CCONS_STR)))
	{
		if(lookahead_in(CMULT | CDIV | CAND))
			scanner();
		else
			error_handler(65);
		factor(folset | CMULT | CDIV | CAND | (CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR
			| CCONS_STR));
	}
}


void factor(set folset)
{
    test((CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR), folset, 57);
	switch(lookahead())
	{
		case CIDENT:
			/***************** Re-hacer *****************/
			if(sbol->lexema[0] == 'f')
				llamada_funcion(folset);
			else
				variable(folset);
			/********************************************/
			/* El alumno debera evaluar con consulta a TS
			si bifurca a variable o llamada a funcion */
			break;

		case CCONS_ENT:
		case CCONS_FLO:
		case CCONS_CAR:
			constante(folset);
			break;

		case CCONS_STR:
			scanner();
			break;

		case CPAR_ABR:
			match(CPAR_ABR, 20);
			expresion(folset | CPAR_CIE);
			match(CPAR_CIE, 21);
			break;

		case CNEG:
			scanner();
			expresion(folset);
			break;

		default:
			error_handler(32);
	}

	test(folset, NADA, 58);
}


void variable(set folset)
{
	test(CIDENT, (folset | CCOR_ABR | (CMAS | CMENOS | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR)), 59);
	match(CIDENT, 17);

	/* El alumno debera verificar con una consulta a TS
	si, siendo la variable un arreglo, corresponde o no
	verificar la presencia del subindice */

	if(lookahead_in(CCOR_ABR))
	{
		match(CCOR_ABR, 35);
		expresion(folset | CCOR_CIE);
		match(CCOR_CIE, 22);
	}
	test(folset, NADA, 60);
}


void llamada_funcion(set folset)
{
	match(CIDENT, 17);

	match(CPAR_ABR, 20);

	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		lista_expresiones(folset | CPAR_CIE);

	match(CPAR_CIE, 21);

	test(folset, NADA, 61);
}


void lista_expresiones(set folset)
{
	expresion(folset | CCOMA | (CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR));

	while(lookahead_in(CCOMA | (CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR)))
	{
		match(CCOMA, 64);
		expresion(folset | CCOMA | (CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR));
	}
}


void constante(set folset)
{
	test((CCONS_ENT | CCONS_FLO | CCONS_CAR), folset, 62);

	switch(lookahead())
	{
		case CCONS_ENT:
			scanner();
			break;

		case CCONS_FLO:
			scanner();
			break;

		case CCONS_CAR:
			scanner();
			break;

		default:
			error_handler(33);
	}

	test(folset, NADA, 63);
}
