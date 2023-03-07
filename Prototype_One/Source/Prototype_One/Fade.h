// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"

/**
 * 
 */
class PROTOTYPE_ONE_API Fade 
{
	enum class FadeState
	{
		FadeIn,
		FadeOut,
		Halt,
	};
	UImage* imageToFade;
	float fadeTime;
	float fadeTimer;
	float fadeValue = 0;
	FadeState fadeState = FadeState::Halt;
public:
	Fade(UImage* _imageToFade, float _fadeTime);
	void Tick(float deltaTime);
	void FadeIn();
	void FadeOut();
	void StopFade();
	void ImmediateFadeOut();
	void ImmediateFadeIn();
	~Fade();
};
