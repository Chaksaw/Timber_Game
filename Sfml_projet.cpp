// Inclure les bibliothèques importantes ici
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>

// Rendre le code plus facile à saisir avec "en utilisant l'espace de noms"
using namespace sf;
using namespace std;

// Fonction pour sauvegarder les scores
void saveScore(const string& playerName, int score) {
	try {
		ofstream scoreFile("scores.txt", ios::app);
		if (!scoreFile.is_open()) {
			throw ios_base::failure("Impossible d'ouvrir scores.txt pour écriture.");
		}

		// Obtenir la date et l'heure actuelles
		time_t now = time(nullptr);
		tm timeInfo;
		if (localtime_s(&timeInfo, &now) != 0) {
			throw runtime_error("Erreur lors de la conversion de l'heure locale.");
		}

		char timeBuffer[100];
		strftime(timeBuffer, sizeof(timeBuffer), "%a %b %d %H:%M:%S %Y", &timeInfo);

		// Écrire les informations dans le fichier
		scoreFile << "Timber Scores on : " << timeBuffer << '\n';
		scoreFile << "Player Name : " << playerName << "\t\tScore : " << score << "\n\n";

		scoreFile.close();
	}// Envoie de l'exception dans le fichier log
	catch (const exception& e) {
		cerr << "Erreur lors de la sauvegarde du score : " << e.what() << '\n';
		ofstream logFile("log.txt", ios::app);
		logFile << "Erreur lors de la sauvegarde du score : " << e.what() << '\n';
	}
}


// Déclaration de fonction
void updateBranches(int seed);

const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];

// Où est le joueur/branche ?
// Gauche ou Droite
enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES];


// C'est ici que notre jeu commence à partir de int main()
int main()
{
	// Détection de la résolution de l'écran

	// Créer un objet VideoMode
	VideoMode vm = sf::VideoMode::getDesktopMode();
	float aspectRatio = 16.0f / 9.0f;
	unsigned int width = vm.width;
	unsigned int height = vm.height;
	

	// Code supplémentaire pour prendre des captures d'écran
	RenderTexture renderTexture;
	renderTexture.create(width, height);
	bool shotTaken = true;
	////////////////////////////////////

	// Fichier de sortie pour les erreurs
	ofstream logFile("log.txt", ios::app);

	// Créer et ouvrir une fenêtre pour le jeu
	
	RenderWindow window;
	try {
		window.create(vm, "Timber!!!", sf::Style::Fullscreen);
	}
	catch (const exception& e) {
		logFile << "Erreur lors de la création de la fenêtre : " << e.what() << '\n';
		cerr << "Erreur détectée. Consultez log.txt pour plus d'informations.\n";
		return -1;
	}
	window.setKeyRepeatEnabled(false);
	// Charger la texture de l'arrière-plan

	// Créer une texture pour contenir un graphique
	Texture textureBackground;
	try {
		// Charger un graphique dans la texture
		if (!textureBackground.loadFromFile("graphics/background.png")) {
			throw runtime_error("Impossible de charger graphics/background.png.");
		}
	}
	catch (const exception& e) {
		logFile << "Erreur lors du chargement de graphics/background.png : " << e.what() << '\n';
		cerr << "Erreur détectée. Consultez log.txt pour plus d'informations.\n";
		return -1;
	}
	// Créer un sprite
	Sprite spriteBackground;

	// Attachez la texture au sprite 
	spriteBackground.setTexture(textureBackground);

	// Définir le spriteBackground pour couvrir l'écran
	spriteBackground.setPosition(0, 0);



	// Charger les textures et sprites pour l'arbre avec gestion d'erreur

	Texture textureTree;
	try {
		if (!textureTree.loadFromFile("graphics/tree.png")) {
			throw runtime_error("Impossible de charger graphics/tree.png.");
		}
	}
	catch (const exception& e) {
		logFile << "Erreur lors du chargement de graphics/tree.png : " << e.what() << '\n';
		cerr << "Erreur détectée. Consultez log.txt pour plus d'informations.\n";
		return -1;
	}
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);	
	

	// Charger la texture de l'abeille avec gestion d'erreur
	Texture textureBee;
	try {
		if (!textureBee.loadFromFile("graphics/bee.png")) {
			throw runtime_error("Impossible de charger graphics/bee.png.");
		}
	}
	catch (const exception& e) {
		logFile << "Erreur lors du chargement de graphics/bee.png : " << e.what() << '\n';
		cerr << "Erreur détectée. Consultez log.txt pour plus d'informations.\n";
		return -1;
	}
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(0, 800);

	// L'abeille bouge-t-elle actuellement ?
	bool beeActive = false;

	// À quelle vitesse l'abeille peut-elle voler
	float beeSpeed = 0.0f;

	// Charger les textures pour les nuages avec gestion d'erreur
	Texture textureCloud;
	try {
		if (!textureCloud.loadFromFile("graphics/cloud.png")) {
			throw runtime_error("Impossible de charger graphics/cloud.png.");
		}
	}
	catch (const exception& e) {
		logFile << "Erreur lors du chargement de graphics/cloud.png : " << e.what() << '\n';
		cerr << "Erreur détectée. Consultez log.txt pour plus d'informations.\n";
		return -1;
	}

	const int numClouds = 5;
	vector<sf::Sprite> clouds(numClouds);
	vector<float> cloudSpeeds(numClouds);
	vector<bool> cloudActive(numClouds);

	for (int i = 0; i < numClouds; ++i) {
		clouds[i].setTexture(textureCloud);
		cloudActive[i] = false;
	}

	// Charger la texture de l'arbre d'arrière-plan avec gestion d'erreur 
	Texture bgTreeTexture;
	try {
		if (!bgTreeTexture.loadFromFile("graphics/bgTree.png")) {
			throw runtime_error("Impossible de charger graphics/bgTree.png.");
		}
	}
	catch (const exception& e) {
		logFile << "Erreur lors du chargement de graphics/bgTree.png : " << e.what() << '\n';
		cerr << "Erreur détectée. Consultez log.txt pour plus d'informations.\n";
		return -1;
	}

	// Gestions des arbres de background 
	vector<sf::Sprite> trees;
	for (int i = 0; i < 5; ++i) {
		sf::Sprite tree(bgTreeTexture);
		float xPos = static_cast<float>(rand() % width);
		float yPos = static_cast<float>(height / 4) - 100;
		tree.setPosition(xPos, yPos);
		trees.push_back(tree);
	}


	// Variables pour contrôler le temps
	Clock clock;

	// Barre de temps
	RectangleShape timeBar;
	float timeBarStartWidth = width;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition(0, 980);

	Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

	// Suivre si le jeu est en cours d'exécution
	bool paused = true;

	// Dessine du texte
	int score = 0;
	Text prompt_info_player;
	Text messageText;
	Text scoreText;
	

	// Nous devons choisir une police
	Font font;
	font.loadFromFile("fonts/KOMIKAP_.ttf");

	// Définir la police de notre message
	messageText.setFont(font);
	scoreText.setFont(font);


	// Attribuer le message réel
	messageText.setString("Press Enter to start!");
	scoreText.setString("Score = 0");


	// Faire le texte vraiment grand
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);

	// Choisissez une couleur
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);

	// Positionner le texte
	FloatRect textRect_message = messageText.getLocalBounds();

	messageText.setOrigin(textRect_message.left +
		textRect_message.width / 2.0f,
		textRect_message.top +
		textRect_message.height / 2.0f);

	messageText.setPosition(width / 2.0f, height / 2.0f);

	scoreText.setPosition(20, 20);


	// Texte pour demander le nom
	Text askNameText("Entrez votre nom : ", font, 50);
	askNameText.setFillColor(Color::White);
	askNameText.setPosition(width / 4, height / 3);

	// Texte pour afficher le nom entré
	Text playerNameText("", font, 50);
	playerNameText.setFillColor(Color::Green);
	playerNameText.setPosition((width / 4) + 560, height / 3);

	string playerName;
	bool nameEntered = false;

	// Préparez 5 branches
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");

	// Définir la texture de chaque sprite de branche
	for (int i = 0; i < NUM_BRANCHES; i++) {
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);

		// Place l'origine du sprite au point mort
		// On peut alors le faire tourner sans changer sa position
		branches[i].setOrigin(220, 20);
	}




	// Préparez le joueur
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);

	// Le joueur commence à gauche
	side playerSide = side::LEFT;

	// Préparez la pierre tombale
	Texture textureRIP;
	textureRIP.loadFromFile("graphics/rip.png");
	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(600, 860);

	// Préparez la hache
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);

	// Alignez la hache avec l'arbre
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;

	// Préparez la buche volante
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);

	// Quelques autres variables utiles liées au log 
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	// Control the player input
	bool acceptInput = false;

	// Préparez le son
	SoundBuffer chopBuffer;


	if (!chopBuffer.loadFromFile("sound/chop.wav"))
	{
		cout << "didn't load";
	}

	Sound chop;
	chop.setBuffer(chopBuffer);

	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);

	// Manque de temps (out of time)
	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/out_of_time.wav");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);


	while (window.isOpen() )
	{
		
		/*
		******************************************************
		Gérer les entrées du joueur - Handle the players input
		******************************************************
		*/

		Event event;

		while (window.pollEvent(event))
		{
			if (event.type == Event::TextEntered) {
				char enteredChar = static_cast<char>(event.text.unicode);
				if (enteredChar == '\b') { // Touche Retour arrière
					if (!playerName.empty()) {
						playerName.pop_back();
					}
				}
				else if (enteredChar == '\r') { // Touche Entrée
					if (!playerName.empty()) {
						nameEntered = true;
					}
					else
					{//Nom de joueur par defaut  si aucun nom n'est saisi
						int idGuest = rand() % 10000;
						playerName = "Guest"+to_string(idGuest);
					}
				}
				else if (enteredChar >= 32 && enteredChar <= 126) { // Caractères imprimables
					playerName += enteredChar;
				}
				playerNameText.setString(playerName);
			}

			if (event.type == Event::KeyReleased && !paused)
			{
				// écouter à nouveau les touches enfoncées
				acceptInput = true;

				// cacher la hache
				spriteAxe.setPosition(2000,
					spriteAxe.getPosition().y);
			}
			
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		// Commencer le jeu
		if (Keyboard::isKeyPressed(Keyboard::Return))
		{
			paused = false;

			// Réinitialiser le temps et le score à chaque nouveau matche
			score = 0;
			timeRemaining = 6;



			// Faire disparaître toutes les branches
			for (int i = 1; i < NUM_BRANCHES; i++)
			{
				branchPositions[i] = side::NONE;
			}

			// Assurez-vous que la pierre tombale est cachée
			spriteRIP.setPosition(675, 2000);

			// Déplacez le joueur en position
			spritePlayer.setPosition(580, 720);

			acceptInput = true;

		}

		// S'asssurer que nous acceptons les entrées du joueur
		if (acceptInput)
		{
			// More code here next...
			// Gérer la touche curseur gauche
			if (Keyboard::isKeyPressed(Keyboard::Right))
			{
				// Assurez-vous que le joueur est à droite
				playerSide = side::RIGHT;

				score++;

				// Ajouter au temps restant
				timeRemaining += (2 / score) + .15;

				spriteAxe.setPosition(AXE_POSITION_RIGHT,
					spriteAxe.getPosition().y);

				spritePlayer.setPosition(1200, 720);

				// Mettre à jour les branches
				updateBranches(score);

				// Faire voler la buche
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;


				acceptInput = false;


				// Jouer le son pour couper (chop sound)
				chop.play();

			}

			// Gérer la touche curseur gauche
			if (Keyboard::isKeyPressed(Keyboard::Left))
			{
				// S'assurer que le joueur est à gauche
				playerSide = side::LEFT;

				score++;

				// Ajouter au temps restant
				timeRemaining += (2 / score) + .15;

				spriteAxe.setPosition(AXE_POSITION_LEFT,
					spriteAxe.getPosition().y);

				spritePlayer.setPosition(580, 720);

				// mettre à jour les branches
				updateBranches(score);

				// Faire voler la buche
				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;


				acceptInput = false;


				// Jouer le son pour couper (chop sound)
				chop.play();

			}

		}

		/*
		******************************************
		Mettre à jour la scène - Update the scene
		******************************************
		*/

		if (!paused)
		{


			// Mesurer le temps
			Time dt = clock.restart();

			// Soustraire du temps restant
			timeRemaining -= dt.asSeconds();
			// grossir la barre de temps
			timeBar.setSize(Vector2f(timeBarWidthPerSecond *
				timeRemaining, timeBarHeight));

			if (timeRemaining <= 0.0f) {


				// Mettre le jeu en pause
				paused = true;

				// Changer le message affiché au joueur
				messageText.setString("Out of time!!");

				// Repositionner le texte en fonction de sa nouvelle taille
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top +
					textRect.height / 2.0f);

				messageText.setPosition(width / 2.0f, height / 2.0f);



				// Jouer le son pour le manque de temps (out of time)
				outOfTime.play();

				saveScore(playerName, score);


			}


			// Configurer l'abeille - Setup the bee
			if (!beeActive)
			{

				// Quelle est la vitesse de l'abeille - How fast is the bee
				srand((int)time(0));
				beeSpeed = (rand() % 200) + 200;

				// Quelle est la hauteur de l'abeille - How high is the bee
				srand((int)time(0) * 10);
				float height = (rand() % 500) + 500;
				spriteBee.setPosition(2000, height);
				beeActive = true;

			}
			else
				// Déplacez l'abeille - Move the bee
			{

				spriteBee.setPosition(
					spriteBee.getPosition().x -
					(beeSpeed * dt.asSeconds()),
					spriteBee.getPosition().y);

				// L’abeille a-t-elle atteint le bord gauche de l’écran ? 
				if (spriteBee.getPosition().x < -100)
				{
					// Préparez-le à devenir une toute nouvelle abeille dans la prochaine image
					beeActive = false;
				}
			}

			for (int i = 0; i <5; i++)
			{
				if (!cloudActive[i])
				{

					// Quelle est la vitesse du nuage
					srand((int)time(0) * 10 +(i*10));
					cloudSpeeds[i] = (rand() % 200);

					// Quelle est la hauteur du nuage
					srand((int)time(0) * 10);
					float height_cloud = (rand() % 150 +(i*50));
					clouds[i].setPosition(-200, height_cloud);
					cloudActive[i] = true;
				}
				else
				{
					clouds[i].setPosition(
						clouds[i].getPosition().x +
						(cloudSpeeds[i] * dt.asSeconds()),
						clouds[i].getPosition().y);

					// Le nuage a-t-il atteint le bord droit de l’écran ?
					if (clouds[i].getPosition().x > width)
					{
						// Configurez-le pour qu'il devienne un tout nouveau cloud dans l'image suivante
						cloudActive[i] = false;
					}
				}
				
			}
					
			// Mettre à jour le score :  (ajouter:  #include <sstream>)
			stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());

			// Mise à jour des sprites de branche dans chaque image
			for (int i = 0; i < NUM_BRANCHES; i++)
			{

				float height = i * 150;

				if (branchPositions[i] == side::LEFT)
				{
					// Déplacez le sprite vers la gauche
					branches[i].setPosition(610, height);

					// Retournez le sprite dans l'autre sens (Flip)
					branches[i].setRotation(180);
				}
				else if (branchPositions[i] == side::RIGHT)
				{
					// Déplacez le sprite vers la droite
					branches[i].setPosition(1330, height);

					// Réglez la rotation du sprite sur normale
					branches[i].setRotation(0);

				}
				else
				{
					// Cacher la branche
					branches[i].setPosition(3000, height);
				}
			}

			// S'occuper de la bûche volante					
			if (logActive)
			{


				spriteLog.setPosition(
					spriteLog.getPosition().x +
					(logSpeedX * dt.asSeconds()),

					spriteLog.getPosition().y +
					(logSpeedY * dt.asSeconds()));

				// La bûche a-t-elle atteint le bord droit ?
				if (spriteLog.getPosition().x < -100 ||
					spriteLog.getPosition().x > 2000)
				{
					// Configurez-le prêt à être un tout nouveau journal dans l'image suivante
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}


			// le joueur a-t-il été écrasé par une branche ?
			if (branchPositions[5] == playerSide)
			{
				// la mort
				paused = true;
				acceptInput = false;

				// Dessiner la pierre tombale
				spriteRIP.setPosition(525, 760);

				// Cacher le joueur
				spritePlayer.setPosition(2000, 660);

				// Changer le message Text
				messageText.setString("SQUISHED!!");

				// Centrer à l'écran
				FloatRect textRect = messageText.getLocalBounds();

				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f);

				messageText.setPosition(width / 2.0f,
					height / 2.0f);

				// Jouer le son pour la  mort (death sound)
				death.play();


				saveScore(playerName, score);
			}

		} // End if(!paused)

		/*
		****************************************
		Dessine la scène - Draw the scene
		****************************************
		*/

		// Effacer tout depuis la dernière image - clear
		window.clear();

		// Dessiner notre scène de jeu ici - draw
		window.draw(spriteBackground);
		
		for (const auto& tree : trees) {
			window.draw(tree);
		}

		// Dessiner les nuages
		for (const auto& cloud : clouds) {
			window.draw(cloud);
		}

		// Dessiner les branches
		for (int i = 0; i < NUM_BRANCHES; i++)
		{
			window.draw(branches[i]);
		}


		// Dessiner l'arbre
		window.draw(spriteTree);

		// Dessiner le joueur
		window.draw(spritePlayer);

		// Dessiner la hache
		window.draw(spriteAxe);

		// Dessiner la buche
		window.draw(spriteLog);

		// Dessiner la pierre tombale
		window.draw(spriteRIP);


		// Dessiner l'abeille
		window.draw(spriteBee);

		// Dessiner le score
		window.draw(scoreText);

		// Dessiner la barre de temps
		window.draw(timeBar);


		if (paused)
		{
			//Dessiner le prompt pour le nom du joueur
			window.draw(playerNameText);
			window.draw(askNameText);
			// Dessiner le message
			window.draw(messageText);
		}

		// Afficher tout - display
		window.display();

		// Code supplémentaire pour prendre des captures d'écran
		if (!shotTaken)
		{

			// Effacer la texture
			renderTexture.clear();

			renderTexture.draw(spriteBackground);
			for (const auto& cloud : clouds) {
				renderTexture.draw(cloud);
			}
			
			// Dessine l'arbre
			renderTexture.draw(spriteTree);

			// Dessiner les branches
			for (int i = 0; i < NUM_BRANCHES; i++)
			{
				renderTexture.draw(branches[i]);
			}

			// Dessiner l'abeille
			renderTexture.draw(spriteBee);

			// Dressiner le joueur
			renderTexture.draw(spritePlayer);

			// Dessiner la hache
			renderTexture.draw(spriteAxe);

			// Dessiner la buche
			renderTexture.draw(spriteLog);

			// Dessiner la pierre tombale
			renderTexture.draw(spriteRIP);


			// Dessiner le score
			renderTexture.draw(scoreText);

			// Dessine la barre de temps
			renderTexture.draw(timeBar);

			if (paused)
			{
				// Dessiner le message
				renderTexture.draw(messageText);
			}

			// Mettre à jour la texture
			renderTexture.display();

			// Créez un sf::Image et copiez le contenu de la texture de rendu
			sf::Image screenshot = renderTexture.getTexture().copyToImage();

			// Enregistrez la capture d'écran dans un fichier
			screenshot.saveToFile("screenshot.png");
			shotTaken = true;
		}
	}
	
	return 0;
}

// Définition de la fonction updateBranches
void updateBranches(int seed)
{
	// Déplacez toutes les branches vers le bas d'un endroit
	for (int j = NUM_BRANCHES - 1; j > 0; j--)
	{
		branchPositions[j] = branchPositions[j - 1];
	}

	// Générer une nouvelle branche à la position 0
	// LEFT, RIGHT or NONE
	srand((int)time(0) + seed);
	int r = (rand() % 5);

	switch (r) {
	case 0:
		branchPositions[0] = side::LEFT;
		break;

	case 1:
		branchPositions[0] = side::RIGHT;
		break;

	default:
		branchPositions[0] = side::NONE;
		break;
	}

}