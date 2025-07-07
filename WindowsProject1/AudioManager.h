#pragma once
class AudioManager {
public:
    AudioManager();

    ~AudioManager();

    bool LoadAllSoundsFromFile(const std::string& listFilePath);

    bool LoadSound(const std::string& name, const std::string& filePath, bool loop = false, double volume = 1.0);

    void StartSound(const std::string& name, bool record = true);

    void StopSound(const std::string& name, bool record = true);

    void SetPlaySpeed(const std::string& name, float rate = 1.0);

    void SetVolume(const std::string& name, float volume);

    void Update();
    void PauseSound(const std::string& name);
    void ResumeSound(const std::string& name);
private:
    FMOD::System* system = nullptr;
    std::map<std::string, FMOD::Sound*> sounds;
    std::map<std::string, FMOD::Channel*> channels;
    std::unordered_map<std::string, float> volumeMap;
    std::unordered_map<std::string, float> originalFrequencies;

};