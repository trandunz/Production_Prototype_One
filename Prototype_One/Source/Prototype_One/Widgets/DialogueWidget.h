#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonUtilities.h"
#include "Dom/JsonObject.h"
#include "DialogueWidget.generated.h"

USTRUCT()
struct FOpenAIResponse
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString id;

	UPROPERTY()
	TArray<FString> choices;
};



UCLASS()
class PROTOTYPE_ONE_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category= AI, meta = (AllowPrivateAccess))
	bool EnableAIParaphrasing{};
	static FString RandomHelloMessage;
	FString API_KEY = "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";
	
private:
	static void OnOpenAIResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void SendOpenAIRequest(const FString& _input);
	static void ApplyRegex(const FString& InputString, const FString& RegexString, TArray<FString>& OutMatches);
public:
	virtual void NativeOnInitialized() override;
	
	void ShowDialogueBox(bool);
	UFUNCTION()
	void OnNextDialogueLine();
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* DialogueBackground;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* DialogueText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* Dialogue_NextLine;

	UPROPERTY(EditAnywhere)
	TArray<FString> DialogueLines{};

	int CurrentDialogueIndex{};
};
