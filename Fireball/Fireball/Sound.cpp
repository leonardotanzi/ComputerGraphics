#include "Sound.h"

Sound::Sound(void)
{
	FMOD::System_Create(&system);// create an instance of the game engine
	system->init(32, FMOD_INIT_NORMAL, 0);// initialise the game engine with 32 channels (cantidad de sonido simultaneo que puede haber)
}

Sound::~Sound(void)
{
	for (int i = 0; i<NUM_SOUNDS; i++) sounds[i]->release();
	system->release();
}

bool Sound::Load()
{
	system->createStream("Sounds/Ambient.wav", FMOD_SOFTWARE | FMOD_LOOP_NORMAL, 0, &sounds[SOUND_AMBIENT]);
	system->createSound("Sounds/Gameover.wav", FMOD_SOFTWARE, 0, &sounds[SOUND_GAMEOVER]);
	system->createSound("Sounds/Choose.wav", FMOD_HARDWARE, 0, &sounds[SOUND_CHOOSE]);
	system->createSound("Sounds/Coin.wav", FMOD_HARDWARE, 0, &sounds[SOUND_COIN]);
	system->createSound("Sounds/Lava.wav", FMOD_HARDWARE, 0, &sounds[SOUND_LAVA]);
	system->createSound("Sounds/Water.wav", FMOD_HARDWARE, 0, &sounds[SOUND_WATER]);
	system->createSound("Sounds/Jump.wav", FMOD_HARDWARE, 0, &sounds[SOUND_JUMP]);
	return true;
}

void Sound::Play(int sound_id)
{
	if (sound_id == SOUND_AMBIENT || sound_id == SOUND_GAMEOVER) {
		system->playSound(FMOD_CHANNEL_FREE, sounds[sound_id], false, &ambientChannel);
		ambientChannel->setVolume(0.25f); //between 0 and 1
	}
	else system->playSound(FMOD_CHANNEL_FREE, sounds[sound_id], false, &effectsChannel);
}

void Sound::StopAll()
{
	ambientChannel->stop();
	effectsChannel->stop();
}

void Sound::Update()
{
	system->update();
}