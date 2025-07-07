#include "pch.h"
#include "AudioManager.h"

#include "Engine.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
AudioManager::AudioManager() {
    FMOD::System_Create(&system);
    system->init(512, FMOD_INIT_NORMAL, 0);
}

AudioManager::~AudioManager() {
    for (auto& [_, sound] : sounds) {
        sound->release();
    }
    system->close();
    system->release();
}


bool AudioManager::LoadAllSoundsFromFile(const std::string& listFilePath) {
    std::ifstream file(listFilePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open sound list file: " << listFilePath << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string name, path, loopStr, vol;

        if (!std::getline(iss, name, ',') ||
            !std::getline(iss, path, ',') ||
            !std::getline(iss, loopStr, ',') ||
            !std::getline(iss, vol, ',')
            ) {
            std::cerr << "Malformed line in sound list: " << line << std::endl;
            continue;
        }

        bool loop = (loopStr == "1");
        double volume = std::stod(vol);
        if (!LoadSound(name, path, loop, volume)) {
            std::cerr << "Failed to load sound: " << name << " from " << path << std::endl;
        }
    }

    file.close();
    return true;
}


bool AudioManager::LoadSound(const std::string& name, const std::string& filePath, bool loop, double volume) {
    FMOD_MODE mode = loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT;
    FMOD::Sound* sound = nullptr;
    if (system->createSound(filePath.c_str(), mode, 0, &sound) == FMOD_OK) {
        sounds[name] = sound;
        SetVolume(name, volume);
        return true;
    }
    return false;
}

void AudioManager::StartSound(const std::string& name, bool record) {
    if (sounds.count(name)) {
        system->playSound(sounds[name], 0, false, &channels[name]);

        if (volumeMap.count(name)) {
            channels[name]->setVolume(volumeMap[name]);
        }

        if (record) {
            Engine::GetInstance().GetSceneManager()->activeScene->objectManager->RecordSound(name);
        }
    }
    else
    {
        std::cout << "Can't find sound: " + name << std::endl;
    }
}

void AudioManager::SetPlaySpeed(const std::string& name, float rate)
{
    if (channels.count(name)) {
        float baseFreq = 0.0f;

        // 초기화된 적 없으면 저장
        if (!originalFrequencies.count(name)) {
            channels[name]->getFrequency(&baseFreq);
            originalFrequencies[name] = baseFreq;
        }
        else {
            baseFreq = originalFrequencies[name];
        }

        channels[name]->setFrequency(baseFreq * rate);
    }
}

void AudioManager::StopSound(const std::string& name, bool record) {
    if (channels.count(name)) {
        channels[name]->stop();
    }
}

void AudioManager::SetVolume(const std::string& name, float volume) {
    volumeMap[name] = volume;

    if (channels.count(name)) {
        channels[name]->setVolume(volume);
    }
}
void AudioManager::Update() {
    system->update();
}

void AudioManager::PauseSound(const std::string& name) {
    if (channels.count(name)) {
        bool isPlaying = false;
        channels[name]->isPlaying(&isPlaying);
        if (isPlaying) {
            channels[name]->setPaused(true);
        }
    }
}

void AudioManager::ResumeSound(const std::string& name) {
    if (channels.count(name)) {
        bool isPaused = false;
        channels[name]->getPaused(&isPaused);
        if (isPaused) {
            channels[name]->setPaused(false);
        }
    }
}