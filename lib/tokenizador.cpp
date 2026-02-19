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

    string strNormalizado;

    if (pasarAminuscSinAcentos) {
        strNormalizado = Normalizar(str);
    }
    else {
        strNormalizado = str;
    }

    if(casosEspeciales){
        TokenizarCasosEspeciales(strNormalizado, tokens);
    }
    else{
        string::size_type lastPos = strNormalizado.find_first_not_of(delimiters, 0);
        string::size_type pos = strNormalizado.find_first_of(delimiters, lastPos);

        while (string::npos != pos || string::npos != lastPos) {
            tokens.push_back(strNormalizado.substr(lastPos, pos - lastPos));
            lastPos = strNormalizado.find_first_not_of(delimiters, pos);
            pos = strNormalizado.find_first_of(delimiters, lastPos);
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
        list<string>::iterator it;
        for (it =tokens.begin(); it != tokens.end(); it++) {
            salida<< (*it) << endl;
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

    for(int i=0; i<len; i++){
        //aqui tenemos el caracter del string
        char c=str[i];

        //hay que controlar los blancos aunque no esten en la lista de delimitadores
        bool blanco=(c == ' ' || c == '\n' || c == '\r');
        //vemos si es un delimitador de la lista que tenemos
        bool encontrado = (delimiters.find(c) != string::npos) || blanco;

        if(!encontrado){
            //caso en el que no estamos en un delimitador por lo que se añade el caracter para formar el token
            token += c;
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

            //CASO URLs --> Delimitadores especiales: ?_:/.?&-=#@?
            //se activa comience por SOLO los indicadores de URL ?http:? o ?https:? o ?ftp:? (en minúsculas)
            // seguido por una secuencia de caracteres (incluidos ?_:/.?&-=#@) sin ningún blanco por medio.
            //Finalizará cuando se detecte un delimitador (excepto ?_:/.?&-=#@?) o un blanco (
            if(!saltar && !token.empty()){
                //lo paso a minusculas para poder asi comparar con el prefijo
                string tokenMinusculas=token;
                for(int j=0; j<tokenMinusculas.length(); j++){
                    tokenMinusculas[j]=tolower(tokenMinusculas[j]);
                }

                //hay que comprobar que si c es :, hay que ver que lo que tenemos en el token es solo http, https o ftp
                bool inicio = (tokenMinusculas=="http" || tokenMinusculas=="https" || tokenMinusculas=="ftp");
                bool medioURL = (tokenMinusculas.find("http:")==0 || tokenMinusculas.find("https:")==0 || tokenMinusculas.find("ftp:")==0);
                bool aceptar=false;
                string delimitadoresURL = "_:/.?&-=#@";

                //por ejemplo en http y luego viene el :
                if(inicio && c==':'){
                    //añadimos los : si el siguiente de estos dos puntos no es blanco o no esta en los permitidos de la lista
                    bool validoURL =(isalnum(siguienteC) || delimitadoresURL.find(siguienteC) != string::npos);
                    if(siguienteC !=0 && validoURL){
                        aceptar=true;
                    }
                }
                //caso de que estemos a mitad de la URL (http:) y viene uno valido luego
                else if(medioURL){
                    if(delimitadoresURL.find(c) != string::npos){
                        aceptar=true;
                    }
                }
                //controla el acceso a saltar
                if(aceptar){
                    saltar=true;
                }
            }

            //CASO NUMEROS --> Delimitadores especiales: ?.,?
            //aparecen al principio del término o por el medio y solo acompañado por numeros (no cientificos)
            if(!saltar && (c=='.' || c==',')){
                //hay que ver que sea numero el token
                bool numero= true;
                if(token.empty()){
                    numero=false;
                }
                else{
                    //aqui se ve lo que llevamos
                    for (int j=0; j<token.length(); j++){
                        if(!isdigit(token[j]) && token[j]!='.' && token[j]!=','){
                            numero=false;
                            //como tiene que ser completo con que haya uno que no lo sea podemos parar
                            break;
                        }
                    }
                }

                //hay que ver que el resto si sean numeros ya que se puede confundir con un acronimo
                bool resto=true;
                for(int k=i+1; k<len; k++){
                    bool blancoNumero=(str[k] == ' ' || str[k] == '\n' || str[k] == '\r');
                    bool delimitadorNumero=(delimiters.find(str[k]) != string::npos) || blancoNumero;
                    //si tenemos un delimitador que no sea , o . ya no puede ser un numero
                    if(delimitadorNumero && str[k]!='.' && str[k]!=','){
                        break;
                    }
                    //se encuentra algo que no es un digito ni un separador, por lo que ya no es un numero
                    if(!isdigit(str[k]) && !delimitadorNumero){
                        resto=false;
                        break;
                    }
                }

                if(resto){
                    //ejemplo 3.5
                    if(numero && isdigit(siguienteC)){
                        // se tiene que almacenar el delimitador tmbn
                        saltar=true;
                    }

                    //ejemplo .67, esto tiene que ser 0.35
                    else if (token.empty() && isdigit(siguienteC)){
                        token="0"; //se tiene que poner ese cero
                        saltar=true;
                    }
                }
            }

            //CASO EMAIL --> Delimitadores especiales: ?@?.
            // detectar el @ por el medio de un término siendo éste delimitador.
            //inicia por un blanco o separador seguido de cualquier carácter, y contiene un solo ?@? por medio
            //detectará el final por la presencia de un espacio en blanco
            if(!saltar){
                if(c=='@'){
                    //la que tenemos actual
                    int numArrobas=1;
                    //solo puede haber un @ en un email por lo que hay que contar ya que si hay dos no se tiene que tratar como un email
                    //en este for lo que hago es contar hacia atras, lo que ya hay en el token
                    for(int k=i-1; k>=0; k--){
                        //si es @ hay que contarla
                        if(str[k]=='@'){
                            numArrobas++;
                        }

                        //si se encuentra un blanco se para
                        bool blancoArroba =(str[k] == ' ' || str[k] == '\n' || str[k] == '\r');
                        if(blancoArroba){
                            break;
                        }

                        //en los emails esta permitido solo los . - y _
                        bool delimitadorPermitido=(str[k] == '.' || str[k] == '-' || str[k] == '_');
                        bool delimitadorArroba=(delimiters.find(str[k]) != string::npos);

                        //si encontramos uno que no es @ ni los permitidos hay que parar
                        if(!delimitadorPermitido && delimitadorArroba && str[k]!='@'){
                            break;
                        }
                    }

                    //antes hemos contado por lo qu ya teniamos de token, ahora hacia delante del @ hasta un blanco
                    for(int k=i+1; k<len; k++){
                        bool blancoArroba =(str[k] == ' ' || str[k] == '\n' || str[k] == '\r');
                        if(blancoArroba){
                            break;
                        }
                        if(str[k]=='@'){
                            numArrobas++;
                        }
                    }

                    //si solo tenemos una arroba hay que saltar
                    if(numArrobas == 1 && !token.empty()){
                        saltar=true;
                    }
                }

                //puede contner tanto _ como -  como . hay que controlar eso ya que es solo despues del @ y rodeados de otro no delimitador y sin ningun blanco
                else if ((c == '.' || c == '-' || c == '_') && !token.empty()){
                    //como he dicho tiene que estar despues de la @, tiene que estar ya en el token
                    if(token.find('@') !=string::npos){
                        //ahora hay que ver lo que tiene rodeado, es decir el siguiente no puede ser delimitador ni blanco
                        bool siguienteDelimitador = (delimiters.find(siguienteC) != string::npos) || (siguienteC == ' ' || siguienteC == '\n' || siguienteC == '\r');
                        if(siguienteC != 0 && !siguienteDelimitador){
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
                //hay que ver que el siguiente no sea un delimitador
                bool siguienteDelimitador=(delimiters.find(siguienteC) != string::npos) || (siguienteC == ' ' || siguienteC == '\n' || siguienteC == '\r');

                //tambien hay que ver que luego haya algo valido
                if(siguienteC != '.' && !siguienteDelimitador && siguienteC !=0){
                    saltar=true;
                }
            }

            //CASO MULTIPALABRAS --> Delimitadores especiales: ?-?.
            //detectar un término que contenga por el medio el carácter ?-? (NO rodeado dedelimitadores o espacios en blanco,
            //detectará el inicio y final de la palabra compuesta cuando aparezca el blanco  o cualquiera de los delimitadores
            if(!saltar && c=='-' && !token.empty()){
                //despues no puede haber algo que sea delimitador
                bool siguienteDelimitador=(delimiters.find(siguienteC) != string::npos) || (siguienteC == ' ' || siguienteC == '\n' || siguienteC == '\r');
                //tampoco puede haber un blanco
                if(siguienteC !=0 && !siguienteDelimitador){
                    saltar=true;
                }
            }

            //logica para añadir o no añadir el delimitador al token
            if(saltar){
                //el delimitador aqui se añade a la palabra ya que ha entrado en un caso especial
                token+=c;
            }
            else{
                //lo que se hace es que si no se ha entrado en ningun caso especial
                //ese delimitador actua como lo que hemos hecho hasta ahora, es decir, es el fin de la palabra y la añadimos al vector para almacenarlas
                if(!token.empty()){
                    tokens.push_back(token);
                    //se reinicia para la siguiente a evaluar
                    token="";
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
