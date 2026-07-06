#include "audio/AudioManager.h"
#include "settings/Settings.h"

AudioManager* AudioManager::instance_ = nullptr;

AudioManager* AudioManager::GetInstance() {
	if (!instance_) {
		instance_ = new AudioManager();
	}
	return instance_;
}

void AudioManager::PlayBgm(const std::string& fileName) {
	// 既に同じ曲が鳴っているなら、再スタートせず音量だけ合わせる（シーン間で途切れさせない）。
	if (bgmPlaying_ && bgmFile_ == fileName) {
		ApplyBgmVolume();
		return;
	}
	StopBgm();
	bgm_ = std::make_unique<TuboEngine::Audio>();
	bgm_->Initialize(fileName);
	bgm_->Play(true, Settings::GetInstance()->bgmVolume);
	bgmFile_ = fileName;
	bgmPlaying_ = true;
}

void AudioManager::StopBgm() {
	if (bgm_) {
		bgm_->Stop();
		bgm_.reset();
	}
	bgmPlaying_ = false;
	bgmFile_.clear();
}

void AudioManager::ApplyBgmVolume() {
	if (bgm_ && bgmPlaying_) {
		bgm_->SetVolume(Settings::GetInstance()->bgmVolume);
	}
}

void AudioManager::PlaySe(const std::string& fileName) {
	auto it = seCache_.find(fileName);
	if (it == seCache_.end()) {
		auto se = std::make_unique<TuboEngine::Audio>();
		se->Initialize(fileName);
		it = seCache_.emplace(fileName, std::move(se)).first;
	}
	// Play は毎回新しいボイスを作るので重ね掛けできる。音量は再生時に渡す。
	it->second->Play(false, Settings::GetInstance()->seVolume);
}
