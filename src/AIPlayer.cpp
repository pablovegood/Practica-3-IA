# include "AIPlayer.h"
# include "Parchis.h"

const double masinf = 9999999999.0, menosinf = -9999999999.0;
const double gana = masinf - 1, pierde = menosinf + 1;
const int num_pieces = 3;
const int PROFUNDIDAD_MINIMAX = 4;  // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA = 6; // Umbral maximo de profundidad para la poda Alfa_Beta

bool AIPlayer::move(){
    cout << "Realizo un movimiento automatico" << endl;

    color c_piece;
    int id_piece;
    int dice;
    think(c_piece, id_piece, dice);

    cout << "Movimiento elegido: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    actual->movePiece(c_piece, id_piece, dice);
    return true;
}

void AIPlayer::think(color & c_piece, int & id_piece, int & dice) const{
    // IMPLEMENTACIÓN INICIAL DEL AGENTE

    // El siguiente código se proporciona como sugerencia para iniciar la implementación del agente.
    
    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha = menosinf, beta = masinf; // Cotas iniciales de la poda AlfaBeta
    // Llamada a la función para la poda (los parámetros son solo una sugerencia, se pueden modificar).
    valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    // ----------------------------------------------------------------- //

    // Si quiero poder manejar varias heurísticas, puedo usar la variable id del agente para usar una u otra.
    switch(id){
        case 0:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
            break;
        case 1:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, pabloHeuristic);
            break;
        case 2:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
            break;
    }
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    
    /*switch(id){
    	case 0:
    		thinkAleatorio(c_piece, id_piece, dice);
    		break;
    	case 1:
    		thinkAleatorioMasInteligente(c_piece, id_piece, dice);
    		break;
    	case 2:
   		thinkFichaMasAdelantada(c_piece, id_piece, dice);
    		break;
    	case 3:
    		thinkMejorOpcion(c_piece, id_piece, dice);
    		break;
    }*/
}

/*virtual*/void AIPlayer::thinkAleatorio(color & c_piece, int & id_piece, int & dice) const{ 
    // IMPLEMENTACIÓN INICIAL DEL AGENTE
    // Esta implementación realiza un movimiento aleatorio.
    // Se proporciona como ejemplo, pero se debe cambiar por una que realice un movimiento inteligente
    //como lo que se muestran al final de la función.

    // OBJETIVO: Asignar a las variables c_piece, id_piece, dice (pasadas por referencia) los valores,
    //respectivamente, de:
    // - color de ficha a mover
    // - identificador de la ficha que se va a mover
    // - valor del dado con el que se va a mover la ficha.

    // El id de mi jugador actual.
    int player = actual->getCurrentPlayerId();

    // Vector que almacenará los dados que se pueden usar para el movimiento
    vector<int> current_dices;
    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<tuple<color, int>> current_pieces;

    // Se obtiene el vector de dados que se pueden usar para el movimiento
    current_dices = actual->getAvailableNormalDices(player);
    // Elijo un dado de forma aleatoria.
    dice = current_dices[rand() % current_dices.size()];

    // Se obtiene el vector de fichas que se pueden mover para el dado elegido
    current_pieces = actual->getAvailablePieces(player, dice);

    // Si tengo fichas para el dado elegido muevo una al azar.
    if (current_pieces.size() > 0)
    {
        int random_id = rand() % current_pieces.size();
        id_piece = get<1>(current_pieces[random_id]); // get<i>(tuple<...>) me devuelve el i-ésimo
        c_piece = get<0>(current_pieces[random_id]);  // elemento de la tupla
    }
    else
    {
        // Si no tengo fichas para el dado elegido, pasa turno (la macro SKIP_TURN me permite no mover).
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }
}

/*virtual*/ void AIPlayer::thinkAleatorioMasInteligente(color & c_piece, int & id_piece, int & dice) const{
	// El número de mi jugador actual.
	int player = actual->getCurrentPlayerId();

	// Vector que almacenará los dados que se pueden usar para el movimiento.
	vector<int> current_dices_con_especiales;

	// Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
	vector<tuple<color, int>> current_pieces;

	// Obtengo el vector de dados que puedo usar para el movimiento.
	// En este caso elijo todos, tanto normales como especiales.
	// Importante: puedo acceder a solo dados normales o especiales por separado,
	// o a todos a la vez:
	// - actual->getAvailableNormalDices(player) -> solo dados normales
	// - actual->getAvailableSpecialDices(player) -> solo dados especiales
	// - actual->getAllAvailableDices(player) -> todos los dados

	// Importante 2: los "available" me dan los dados que puedo usar en el turno actual.
	// Por ejemplo, si me tengo que contar 10 o 20 solo me saldrán los dados 10 y 20.
	// Puedo saber qué más dados tengo, aunque no los pueda usar en este turno, con:
	// - actual->getNormalDices(player) -> todos los dados normales
	// - actual->getSpecialDices(player) -> todos los dados especiales
	// - actual->getAllDices(player) -> todos los dados
	current_dices_con_especiales = actual->getAllAvailableDices(player);
	
	// En vez de elegir un dado al azar, miro primero cuáles tienen fichas que se puedan mover.
	vector<int> current_dices_que_pueden_mover_ficha;
	
	for (int i = 0; i < current_dices_con_especiales.size(); i++)
	{
		// Se obtiene el vector de fichas que se pueden mover para el dado elegido.
		current_pieces = actual->getAvailablePieces(player, current_dices_con_especiales[i]);
		// Si se pueden mover fichas para el dado actual, lo añado al vector de dados que pueden mover fichas.
		if (current_pieces.size() > 0)
		{
			current_dices_que_pueden_mover_ficha.push_back(current_dices_con_especiales[i]);
		}
	}
	
	// Si no tengo ninún dado que pueda mover fichas, paso turno con un dado al azar (la macro SKIP_TURN me permite no mover).
	if (current_dices_que_pueden_mover_ficha.size() == 0)
	{
		dice = current_dices_con_especiales[rand() % current_dices_con_especiales.size()];
		id_piece = SKIP_TURN;
		c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
	}
	
	// En caso contrario, elijo un dado de forma aleatoria de entre los que pueden mover ficha.
	else

	{
		dice = current_dices_que_pueden_mover_ficha[rand() % current_dices_que_pueden_mover_ficha.size()];
		
		// Se obtiene el vector de fichas que se pueden mover para el dado elegido.
		current_pieces = actual->getAvailablePieces(player, dice);
		
		// Muevo una ficha al azar de entre las que se pueden mover.
		int random_id = rand() % current_pieces.size();
		id_piece = get<1>(current_pieces[random_id]);
		c_piece = get<0>(current_pieces[random_id]);
	}
}

/*virtual*/ void AIPlayer::thinkFichaMasAdelantada(color & c_piece, int & id_piece, int & dice) const{
	// Elijo el dado haciendo lo mismo que el jugador anterior.
	thinkAleatorioMasInteligente(c_piece, id_piece, dice);
	// Tras llamar a esta función, ya tengo en dice el número de dado que quiero usar.
	// Ahora, en vez de mover una ficha al azar, voy a mover (o a aplicar
	// el dado especial a) la que esté más adelantada
	// (equivalentemente, la más cercana a la meta).

	int player = actual->getCurrentPlayerId();
	vector<tuple<color, int>> current_pieces = actual->getAvailablePieces(player, dice);

	int id_ficha_mas_adelantada = -1;
	color col_ficha_mas_adelantada = none;
	int min_distancia_meta = 9999;	
	
	for (int i = 0; i < current_pieces.size(); i++)
	{
		// distanceToGoal(color, id) devuelve la distancia a la meta de la ficha [id] del color que le indique.
		color col = get<0>(current_pieces[i]);
		int id = get<1>(current_pieces[i]);
		int distancia_meta = actual->distanceToGoal(col, id);
		if (distancia_meta < min_distancia_meta)
		{
			min_distancia_meta = distancia_meta;
			id_ficha_mas_adelantada = id;
			col_ficha_mas_adelantada = col;
		}
	}

	// Si no he encontrado ninguna ficha, paso turno.
	if (id_ficha_mas_adelantada == -1)
	{
		id_piece = SKIP_TURN;
		c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
	}
	// En caso contrario, moveré la ficha más adelantada.
	else
	{
		id_piece = id_ficha_mas_adelantada;
		c_piece = col_ficha_mas_adelantada;
	}
}

/*virtual*/ void AIPlayer::thinkMejorOpcion(color &c_piece, int &id_piece, int &dice) const{
	// Vamos a mirar todos los posibles movimientos del jugador actual accediendo a los hijos del estado actual.

	// Para ello, vamos a iterar sobre los hijos con la función de Parchis getChildren().
	// Esta función devuelve un objeto de la clase ParchisBros, que es una estructura iterable
	// sobre la que se pueden recorrer todos los hijos del estado sobre el que se llama.

	ParchisBros hijos = actual->getChildren();

	bool me_quedo_con_esta_accion = false;

	// La clase ParchisBros viene con un iterador muy útil y sencillo de usar.
	// Al hacer begin() accedemos al primer hijo de la rama,
	// y cada vez que hagamos ++it saltaremos al siguiente hijo.	
	// Comparando con el iterador end() podemos consultar cuándo hemos terminado de visitar los hijos.
	for(ParchisBros::Iterator it = hijos.begin();
		it != hijos.end() and !me_quedo_con_esta_accion; ++it){
		Parchis siguiente_hijo = *it; // Accedemos al tablero hijo con el operador de indirección.
		if(siguiente_hijo.isEatingMove() or // Si he comido ficha o ...
			siguiente_hijo.isGoalMove() or // ... una ficha ha llegado a la meta o ...
			(siguiente_hijo.gameOver() and siguiente_hijo.getWinner() == this->jugador) // ... he ganado
		)
		{
			me_quedo_con_esta_accion = true;
			c_piece = it.getMovedColor(); // Guardo color de la ficha movida.
			id_piece = it.getMovedPieceId(); // Guardo id de la ficha movida.
			dice = it.getMovedDiceValue(); // Guardo número de dado movido.
		}
	}
	
	// Si he encontrado una acción que me interesa, la guardo en las variables pasadas por referencia.
	// (Ya lo he hecho antes, cuando les he asignado los valores con el iterador).
	
	// Si no, muevo la ficha más adelantada como antes.
	if(!me_quedo_con_esta_accion)
	{
		thinkFichaMasAdelantada(c_piece, id_piece, dice);
	}
}

/*virtual*/double AIPlayer::Poda_AlfaBeta(const Parchis & actual, int jugador, int nivel, const int profundidad_alfabeta, color & c_piece, int & id_piece, int & dice, double alpha, double beta, double (*heuristica)(const Parchis &, int)) const{
    // Es recursividad
    bool esMaximizador = false;
    double nuevo_limite;

    color c_piece_aux;
    int id_piece_aux; 
    int dice_aux;

    // Obtenemos los hijos del nodo actual
    ParchisBros hijos = actual.getChildren();
    
    if(jugador == actual.getCurrentPlayerId())
        esMaximizador = true;

    // Verificar si el nodo es un nodo terminal
    if(nivel != profundidad_alfabeta && !actual.gameOver()){
    for(ParchisBros::Iterator it = hijos.begin();
		it != hijos.end(); ++it){
		    Parchis siguiente_hijo = *it; // Accedemos al tablero hijo con el operador de indirección.
		    c_piece = it.getMovedColor(); // Guardo color de la ficha movida.
			id_piece = it.getMovedPieceId(); // Guardo id de la ficha movida.
			dice = it.getMovedDiceValue(); // Guardo número de dado movido.
          
            nuevo_limite = Poda_AlfaBeta(*it, jugador, nivel+1, profundidad_alfabeta, c_piece_aux, id_piece_aux, dice_aux, alpha, beta, heuristica);
            if(esMaximizador){
                if(nuevo_limite > alpha)
                    alpha = nuevo_limite;
                if(alpha <= beta)
                    return alpha;
            }else{
                if(nuevo_limite < beta)
                    beta = nuevo_limite;
                 if(alpha >= beta)
                    return beta;    
            }    
		}
    }else{
        return heuristica(actual, jugador);
    }
}
    
double AIPlayer::ValoracionTest(const Parchis &estado, int jugador)
{
    // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.

    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
    {
        return gana;
    }
    else if (ganador == oponente)
    {
        return pierde;
    }
    else
    {
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++)
        {
            color c = my_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                // Valoro positivamente que la ficha esté en casilla segura o meta.
                if (estado.isSafePiece(c, j))
                {
                    puntuacion_jugador++;                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_jugador += 5;
                }
            }
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++)
        {
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                if (estado.isSafePiece(c, j))
                {
                    // Valoro negativamente que la ficha esté en casilla segura o meta.
                    puntuacion_oponente++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_oponente += 5;
                }
            }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}

double AIPlayer::pabloHeuristic(const Parchis & estado, int jugador){
    
    // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.
    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
    {
        return gana;
    }
    else if (ganador == oponente)
    {
        return pierde;
    }
    else
    {
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++)
        {
            color c = my_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {   // MODIFICAR A PARTIR DE AQUI
                if(estado.piecesAtGoal(c) > 0){
                    puntuacion_jugador += estado.piecesAtGoal(c);
                }

                if(estado.piecesAtHome(c) > 0){
                    puntuacion_jugador -= estado.piecesAtHome(c);
                }

                // Valoro positivamente que la ficha esté en casilla segura o meta.
                if (estado.isSafePiece(c, j))
                {
                    puntuacion_jugador+=2;                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_jugador += 5;
                }
            }
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++)
        {
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {   // MODIFICAR A PARTIR DE AQUI
                if(estado.piecesAtGoal(c) > 0){
                    puntuacion_jugador += estado.piecesAtGoal(c);
                }

                if(estado.piecesAtHome(c) > 0){
                    puntuacion_jugador -= estado.piecesAtHome(c);
                }

                if (estado.isSafePiece(c, j))
                {
                    // Valoro negativamente que la ficha esté en casilla segura o meta.
                    puntuacion_oponente += 2;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_oponente += 5;
                }
            }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}