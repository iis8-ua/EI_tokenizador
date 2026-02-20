#include "tokenizador.h"

Tokenizador::Tokenizador (){
    delimiters=",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t@";
    casosEspeciales=true;
    pasarAminuscSinAcentos=false;
}

Tokenizador::Tokenizador (const Tokenizador& t){
    this->delimiters=t.delimiters;
    this->casosEspeciales=t.casosEspeciales;
    this->pasarAminuscSinAcentos=t.pasarAminuscSinAcentos;
}

Tokenizador::Tokenizador (const string& delimitadoresPalabra, const bool& kcasosEspeciales, const bool& minuscSinAcentos){
    this->casosEspeciales=kcasosEspeciales;
    this->pasarAminuscSinAcentos=minuscSinAcentos;

    //esto es provisional, hay que utilizar una variable auxiliar que recorre los delimitadores de entrada y si no esta el char
    //en la auxiliar se añade y por ultimo hacemos this->delimiters=aux; aux se inicializa con delimiters al inicio
    DelimitadoresPalabra(delimitadoresPalabra);
}

Tokenizador::~Tokenizador (){
    delimiters="";
}

Tokenizador& Tokenizador::operator= (const Tokenizador& t){
    if(this != &t){
        this->delimiters=t.delimiters;
        this->casosEspeciales=t.casosEspeciales;
        this->pasarAminuscSinAcentos=t.pasarAminuscSinAcentos;
    }
    return *this;
}

void Tokenizador::Tokenizar (const string& str, list<string>& tokens) const{
    //versión simple vista en teoria, esta no se toca y no se mide la eficiencia
    tokens.clear();
    if(str.empty()){
        return;
    }
    string strNormalizado;
    //se usa un puntero para que al normalizar se apunta a la copia y asi si es falso pasarAMinusculas se apunta al original y no se copia de nuevo
    const string* punteroStr = &str;

    if (pasarAminuscSinAcentos) {
        strNormalizado = Normalizar(str);
        punteroStr = &strNormalizado;
    }

    if(casosEspeciales){
        TokenizarCasosEspeciales(*punteroStr, tokens);
    }
    else{
        string::size_type lastPos = punteroStr->find_first_not_of(delimiters, 0);
        string::size_type pos = punteroStr->find_first_of(delimiters, lastPos);

        while (string::npos != pos || string::npos != lastPos) {
            tokens.push_back(punteroStr->substr(lastPos, pos - lastPos));
            lastPos = punteroStr->find_first_not_of(delimiters, pos);
            pos = punteroStr->find_first_of(delimiters, lastPos);
        }
    }

}

bool Tokenizador::Tokenizar (const string& i, const string& f) const{
    ifstream entrada;
    ofstream salida;
    string cadena;
    list<string> tokens;

    entrada.open(i.c_str());
    if (!entrada) {
        cerr << "ERROR: No existe el archivo: " << i << endl;
        return false;
    }

    salida.open(f.c_str());
    if (!salida) {
        cerr << "ERROR: No se pudo crear el archivo: " << f << endl;
        entrada.close();
        return false;
    }

    while (getline(entrada, cadena)) {
        //la llamamos para cada línea ya que esta vacia la lista
        Tokenizar(cadena, tokens);

        //se van escribiendo en el fichero de salida, uno por cada linea
        //se ha cambiado it++ por ++it que asi se evita crear una copia temporal del iterador antes de incrementarlo, desperdicio de CPU para bucles largos
        //tambien he cambiado endl por \n para que asi el disco duro trabaje a su velocidad y evitar el flush al disco
        list<string>::iterator it;
        for (it =tokens.begin(); it != tokens.end(); ++it) {
            salida<< (*it) << '\n';
        }
    }
    entrada.close();
    salida.close();
    return true;
}

bool Tokenizador::Tokenizar (const string & i) const{
    string f = i + ".tk";
    return Tokenizar(i, f);
}

bool Tokenizador::TokenizarListaFicheros (const string& i) const{
    ifstream lista;
    string nombreFichero;
    bool todoCorrecto = true;

    lista.open(i.c_str());
    if (!lista) {
        cerr << "ERROR: No existe el archivo de lista: " << i << endl;
        return false;
    }

    while (getline(lista, nombreFichero)) {
        if (nombreFichero.empty()){
            continue;
        }

        //ignorar si es un directorio
        struct stat dir;
        int err=stat(nombreFichero.c_str(), &dir);
        if(err==0){
            if(S_ISDIR(dir.st_mode)){
                continue;
            }
        }

        //se llama a la que va uno por uno (la que añade la extension tk, que se encarga de llamar a la otra) para que vaya tokenizando fichero por fichero
        if (!Tokenizar(nombreFichero)) {
            todoCorrecto = false;
        }
    }

    lista.close();
    return todoCorrecto;
}

bool Tokenizador::TokenizarDirectorio (const string& i) const{
    struct stat dir;
    int err=stat(i.c_str(), &dir);

    //ISDIR comprueba si es un directorio
    if(err==-1 || !S_ISDIR(dir.st_mode)){
        cerr << "ERROR: No existe el directorio o no es un directorio: " << i << endl;
        return false;
    }
    else {
        // Hago una lista en un fichero con find>fich, es una lista con todos los archivos y la guarda en el fichero temporal
        //con follow aseguras que sigue enlaces simbolicos
        string cmd="find "+i+" -follow |sort > .lista_fich";
        system(cmd.c_str());
        return TokenizarListaFicheros(".lista_fich");
    }
}

void Tokenizador::DelimitadoresPalabra(const string& nuevoDelimiters){
    delimiters="";
    for (int i=0; i<nuevoDelimiters.length(); i++){
        char c=nuevoDelimiters[i];
        if(delimiters.find(c) == string::npos){
            delimiters +=c;
        }
    }
}

void Tokenizador::AnyadirDelimitadoresPalabra(const string& nuevoDelimiters){
    for (int i=0; i<nuevoDelimiters.length(); i++){
        char c=nuevoDelimiters[i];
        if(delimiters.find(c) == string::npos){
            delimiters +=c;
        }
    }
}

string Tokenizador::DelimitadoresPalabra() const{
    return delimiters;
}

void Tokenizador::CasosEspeciales (const bool& nuevoCasosEspeciales){
    this->casosEspeciales=nuevoCasosEspeciales;
}

bool Tokenizador::CasosEspeciales (){
    return casosEspeciales;
}

void Tokenizador::PasarAminuscSinAcentos (const bool& nuevoPasarAminuscSinAcentos){
    this->pasarAminuscSinAcentos=nuevoPasarAminuscSinAcentos;
}

bool Tokenizador::PasarAminuscSinAcentos (){
    return pasarAminuscSinAcentos;
}

ostream& operator<<(ostream& os, const Tokenizador& t){
     os<< "DELIMITADORES: " << t.delimiters << " TRATA CASOS ESPECIALES: " << t.casosEspeciales << " PASAR A MINUSCULAS Y SIN ACENTOS: " << t.pasarAminuscSinAcentos;
     return os;
}

void Tokenizador::TokenizarCasosEspeciales(const string& str, list<string>& tokens) const{
    string token ="";
    int len= str.length();

    //se crea este array estatico de booleands para evitar llamar a find, ya que find lo que hace es un bucle dentro del string
    //y como los caracteres van del 0 al 255 se hace de ese tamaño y ahora para ver si es delimitador se consulta aqui, donde si esta a true lo que hace es que es delimitador
    bool es_delim[256]={false};

    //los blancos son delimitadores siempre
    es_delim[(unsigned char)' '] = true;
    es_delim[(unsigned char)'\n'] = true;
    es_delim[(unsigned char)'\r'] = true;

    //se meten los delimitadores en el array
    for (int i = 0; i < delimiters.length(); i++) {
        es_delim[(unsigned char)delimiters[i]] = true;
    }

    //para evitar hacer bucles anidados y que me hacian tener esa complejidad cuadratica,
    //creo una maquina de estados para memorizar el estado

    //para las @ solo habra que comprobar que es == 0, si es asi la añado al token y le sumo uno a esta variable, si se encuantra otra mas delante esta variable va a ser 1 y se rechaza (delimitador normal)
    int arrobas_en_token=0;
    int fin_palabra_email = -1;
    bool es_email_valido = false;
    bool token_tiene_delimitador = false;

    //Para los numeros se hace lo mismo, este bool se actualiza letra a letra, donde si llevo un numero y viene un . seguido de un digito lo añado y sigo,
    //pero si vienen letras despues de un punto, se pone esto a false y se deja de considerar un numero
    bool token_es_numero=true;
    int fin_palabra_num = -1;
    bool resto_es_numero = true;

    //Para poder controlar que es una URL
    bool es_url=false;

    for(int i=0; i<len; i++){
        //aqui tenemos el caracter del string
        char c=str[i];
        //se pasa a unsigned para trabajar y buscar el delimitador en el array estatico
        unsigned char uc = (unsigned char)c;
        //asi la complejidad es O(1) en vez de delimiters.find
        bool encontrado =es_delim[uc];

        if(!encontrado){
            //caso en el que no estamos en un delimitador por lo que se añade el caracter para formar el token
            token += c;
            //se tiene que ir actualizando la maquina de estados en cada iteracion
            if(c=='@'){
                arrobas_en_token++;
            }
            if(!isdigit(uc)){
                token_es_numero=false;
            }
        }
        else{
            //si que es un delimitador, por lo que hay que ver como dice en el enunciado si hay que meterlo o no en el token
            //porque nos dice que el orden de los ifs es importante por lo que un delimitador puede que sea parte del token teniendo
            //en cuenta esos ifs

            //este booleano es el que va a hacer que se mantega o no el delimitador en el token
            bool saltar=false;
            //esta lo que va a tener es el siguiente caracter del string para luego poder usarlo en los diferentes casos especiales
            char siguienteC;
            if(i+1<len){
                siguienteC=str[i+1];
            }
            else{
                //es como si hubiera un blanco ya que se acaba el string, se hace asi para cortar
                siguienteC=0;
            }
            //tambien para el array de busqueda
            unsigned char usiguienteC =(unsigned char) siguienteC;

            //CASO URLs --> Delimitadores especiales: ?_:/.?&-=#@?
            //se activa comience por SOLO los indicadores de URL ?http:? o ?https:? o ?ftp:? (en minúsculas)
            // seguido por una secuencia de caracteres (incluidos ?_:/.?&-=#@) sin ningún blanco por medio.
            //Finalizará cuando se detecte un delimitador (excepto ?_:/.?&-=#@?) o un blanco (
            if(!saltar && !token.empty()){
                //si aun no es una URL y viene los : a continuación
                if(!es_url && c==':'){
                    //lo paso a minusculas para poder asi comparar con el prefijo, antes se hacia copia, ahora lo creo vacio y lo relleno con las minusculas del original
                    string tokenMinusculas = "";
                    for (int j = 0; j < token.length(); j++) {
                        tokenMinusculas += tolower(token[j]);
                    }

                    //hay que comprobar que si c es :, hay que ver que lo que tenemos en el token es solo http, https o ftp
                    if(tokenMinusculas=="http" || tokenMinusculas=="https" || tokenMinusculas=="ftp"){
                        //aqui vuelvo a no usar el find que recorre el string por un switch con los caracteres delimitadores del find
                        bool es_delim_url = false;
                        switch (siguienteC) {
                            case '_': case ':': case '/': case '.': case '?':
                            case '&': case '-': case '=': case '#': case '@':
                                es_delim_url = true;
                                break;
                        }
                        //añadimos los : si el siguiente de estos dos puntos no es blanco o no esta en los permitidos de la lista
                        bool validoURL = (isalnum(usiguienteC) || es_delim_url);
                        if (siguienteC != 0 && validoURL) {
                            es_url = true;
                            saltar = true;
                        }
                    }
                }
                else if(es_url){
                    //se vuelve a hacer lo mismo para evitar el find
                    bool es_delim_url = false;
                    switch (c) {
                        case '_': case ':': case '/': case '.': case '?':
                        case '&': case '-': case '=': case '#': case '@':
                            es_delim_url = true;
                            break;
                    }

                    if (es_delim_url) {
                        saltar = true;
                    }
                    else {
                        es_url = false;
                    }
                }
            }

            //CASO NUMEROS --> Delimitadores especiales: ?.,?
            //aparecen al principio del término o por el medio y solo acompañado por numeros (no cientificos)
            if(!saltar && (c=='.' || c==',')){
                //hay que ver que sea numero el token y que no este vacio
                bool numero= (token_es_numero && !token.empty());

                //antes teniamos un bucle que miraba lo que ya teniamos generando complejidad cuadratica, por lo que se elimina y se hace una vez por palabra
                if(i>=fin_palabra_num){
                    resto_es_numero=true;
                    fin_palabra_num=i+1;

                    //ahora hay que ver el resto hasta el final de la palabra ya que se puede confundir con un acronimo
                    while(fin_palabra_num<len){
                        char sig=str[fin_palabra_num];
                        bool blancoNumero = (sig == ' ' || sig == '\n' || sig == '\r');
                        bool delimitadorNumero = es_delim[(unsigned char)sig] || blancoNumero;

                         //si tenemos un delimitador que no sea , o . ya no puede ser un numero
                        if(delimitadorNumero && sig != '.' && sig != ','){
                            break;
                        }

                        //se encuentra algo que no es un digito ni un separador, por lo que ya no es un numero
                        if(!isdigit((unsigned char)sig) && !delimitadorNumero){
                            //se pone a false para que llegue al final y no se vuelva a escanear esa palabra
                            resto_es_numero=false;
                        }
                        fin_palabra_num++;
                    }
                }

                if(resto_es_numero){
                    //ejemplo 3.5
                    if (numero && isdigit(usiguienteC)) {
                        saltar = true;
                    }
                    //ejemplo .67 -> 0.67
                    else if (token.empty() && isdigit(usiguienteC)){
                        token="0";
                        saltar=true;
                        token_es_numero=true;
                    }
                }
            }

            //comun para los casos siguientes
            bool siguienteDelimitador;
            if(siguienteC==0){
                siguienteDelimitador=true;
            }
            else{
                siguienteDelimitador=es_delim[usiguienteC];
            }

            //CASO EMAIL --> Delimitadores especiales: ?@?.
            // detectar el @ por el medio de un término siendo éste delimitador.
            //inicia por un blanco o separador seguido de cualquier carácter, y contiene un solo ?@? por medio
            //detectará el final por la presencia de un espacio en blanco
            if(!saltar){
                //solo puede haber un @ en un email por lo que hay que contar ya que si hay dos no se tiene que tratar como un email
                if(c=='@'){
                    //si no esta vacio y es la primera arroba hace que mirar hacia atras tenga complejidad O(1), tampoco tiene que tener delimitadores previos
                    if(arrobas_en_token ==0 && !token.empty() && !token_tiene_delimitador){
                        //ahora se mira hacia delante sin escanearlo muchas veces, haciendo que sea complejidad O(N)
                        if(i>=fin_palabra_email){
                            es_email_valido=true;
                            fin_palabra_email=i+1;
                            int contar_arrobas=1; //se cuenta la actual

                            //ahora hasta el final de la palabra
                            while(fin_palabra_email<len){
                                char sig = str[fin_palabra_email];
                                //si se encuentra un blanco se para
                                bool blancoArroba=(sig == ' ' || sig == '\n' || sig == '\r');
                                if (blancoArroba){
                                    break;
                                }

                                if(sig=='@'){
                                    contar_arrobas++;
                                }
                                else{
                                    //si se encuentra uno fuerte que no esta permitido se para ya de mirar
                                    bool es_del = es_delim[(unsigned char)sig];
                                    if (es_del && sig != '.' && sig != '-' && sig != '_') {
                                         break;
                                    }
                                }
                                fin_palabra_email++;
                            }

                            //si hay mas de una ya no es un email
                            if(contar_arrobas !=1){
                                es_email_valido=false;
                            }
                        }
                        if(es_email_valido){
                            saltar=true;
                        }
                    }
                }

                //puede contner tanto _ como -  como . hay que controlar eso ya que es solo despues del @ y rodeados de otro no delimitador y sin ningun blanco
                else if (!token.empty()){
                    //se hace para evitar el find como en los demas
                    bool es_delim_email=false;
                    switch(c){
                        case '.': case '-': case '_':
                            es_delim_email = true;
                            break;
                    }

                    if(es_delim_email && arrobas_en_token >0){
                        //ahora hay que ver lo que tiene rodeado, es decir el siguiente no puede ser delimitador ni blanco
                        if(!siguienteDelimitador){
                            //si se cumple que el siguiente no es blanco y no es delimitador lo incluimos
                            saltar=true;
                        }
                    }
                }
            }

            //CASO ACRONIMOS --> Delimitadores especiales: ?.?
            //se activa cuando se detecte un punto en medio sin ningun blanco y separados por caracteres distintos no vale otro punto
            //para acabar blanco, delimitador o varios puntos seguidos
            if(!saltar && c== '.' && !token.empty()){
                //tambien hay que ver que luego haya algo valido
                if(siguienteC != '.' && !siguienteDelimitador){
                    saltar=true;
                }
            }

            //CASO MULTIPALABRAS --> Delimitadores especiales: ?-?.
            //detectar un término que contenga por el medio el carácter ?-? (NO rodeado dedelimitadores o espacios en blanco,
            //detectará el inicio y final de la palabra compuesta cuando aparezca el blanco  o cualquiera de los delimitadores
            if(!saltar && c=='-' && !token.empty()){
                if(!siguienteDelimitador){
                    saltar=true;
                }
            }

            //logica para añadir o no añadir el delimitador al token
            if(saltar){
                //el delimitador aqui se añade a la palabra ya que ha entrado en un caso especial
                token+=c;
                //ya tiene un delimitador
                token_tiene_delimitador=true;

                //hay que seguir manteniendo los estados actualizados
                if(c=='@'){
                    arrobas_en_token++;
                }
                if(!isdigit(uc) && c!='.' && c!=','){
                    token_es_numero=false;
                }
            }
            else{
                //lo que se hace es que si no se ha entrado en ningun caso especial
                //ese delimitador actua como lo que hemos hecho hasta ahora, es decir, es el fin de la palabra y la añadimos al vector para almacenarlas
                if(!token.empty()){
                    tokens.push_back(token);
                    //se reinicia para la siguiente a evaluar
                    token="";

                    //se reinicia tambien la maquina de estados para la nueva palabra
                    arrobas_en_token = 0;
                    token_es_numero = true;
                    es_url = false;
                    token_tiene_delimitador=false;
                }
            }
        }
    }

    //no se si es necesario y se puede omitir, es por si se queda algo en el token
    if(!token.empty()){
        tokens.push_back(token);
    }
}

string Tokenizador::Normalizar(const string& str) const {
    string aux = str;
    for (int i = 0; i < aux.length(); i++) {
        unsigned char c = (unsigned char)aux[i];

        if ((c >= 0xC0 && c <= 0xC5) || (c >= 0xE0 && c <= 0xE5)) {
            aux[i] = 'a';
        }
        else if ((c >= 0xC8 && c <= 0xCB) || (c >= 0xE8 && c <= 0xEB)) {
            aux[i] = 'e';
        }
        else if ((c >= 0xCC && c <= 0xCF) || (c >= 0xEC && c <= 0xEF)) {
            aux[i] = 'i';
        }
        else if ((c >= 0xD2 && c <= 0xD6) || (c >= 0xF2 && c <= 0xF6)) {
            aux[i] = 'o';
        }
        else if ((c >= 0xD9 && c <= 0xDC) || (c >= 0xF9 && c <= 0xFC)) {
            aux[i] = 'u';
        }
        else if (c == 0xD1) {
            aux[i] = (char)0xF1; //se pasa se Ñ a ñ
        }
        else if (c == 0xC7){
            aux[i] = (char)0xE7; //se pasa de Ç a ç
        }
        else if (c >= 'A' && c <= 'Z'){
            aux[i] = tolower(c); //una vez ya se han quitado las tildes se pasa a minusculas
        }
    }
    return aux;
}
