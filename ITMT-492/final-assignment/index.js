/**
 * This game is Simon game with Alexa
 * ITMT - 492 : Smart Technologies
 * 
 * Jian Kim
 * 
 * */
const Alexa = require('ask-sdk');
const i18n = require('i18next');
const sprintf = require('i18next-sprintf-postprocessor');
const languageStrings = {
  'en': require('./languageStrings')
}

var colors = ["red", "green", "blue", "yellow"];
var score = 0;

const LaunchRequest = {
  canHandle(handlerInput) {
    // launch requests as well as any new session, as games are not saved in progress, which makes
    // no one shots a reasonable idea except for help, and the welcome message provides some help.
    //const { attributesManager } = handlerInput;
    //const sessionAttributes = attributesManager.getSessionAttributes();
    //sessionAttributes.gamesPlayed = 0;
    
    return Alexa.isNewSession(handlerInput.requestEnvelope) 
      || Alexa.getRequestType(handlerInput.requestEnvelope) === 'LaunchRequest';
  },
  async handle(handlerInput) {
    const { attributesManager } = handlerInput;
    const requestAttributes = attributesManager.getRequestAttributes();
    const attributes = await attributesManager.getPersistentAttributes() || {};
    
    if (Object.keys(attributes).length === 0) {
      attributes.endedSessionCount = 0;
      attributes.gameState = 'ENDED';
    }
    
    attributesManager.setSessionAttributes(attributes);
    
    const speechOutput = requestAttributes.t('LAUNCH_MESSAGE');
    const reprompt = requestAttributes.t('CONTINUE_MESSAGE');

    return handlerInput.responseBuilder
      .speak(speechOutput)
      .reprompt(reprompt)
      .getResponse();
  },
};

const ExitHandler = {
  canHandle(handlerInput) {
    return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
      && (Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.CancelIntent'
        || Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.StopIntent');
  },
  handle(handlerInput) {
    const requestAttributes = handlerInput.attributesManager.getRequestAttributes();
    score = 0;
    const sessionAttributes = handlerInput.attributesManager.getSessionAttributes();
    sessionAttributes.guessColor.length = 0;
    sessionAttributes.answerColor.length = 0;
    return handlerInput.responseBuilder
      .speak(requestAttributes.t('EXIT_MESSAGE'))
      .getResponse();
  },
};

const SessionEndedRequest = {
  canHandle(handlerInput) {
    return Alexa.getRequestType(handlerInput.requestEnvelope) === 'SessionEndedRequest';
  },
  handle(handlerInput) {
    console.log(`Session ended with reason: ${handlerInput.requestEnvelope.request.reason}`);
    return handlerInput.responseBuilder.getResponse();
  },
};

const HelpIntent = {
  canHandle(handlerInput) {
    return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest' 
      && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.HelpIntent';
  },
  handle(handlerInput) {
    const requestAttributes = handlerInput.attributesManager.getRequestAttributes();

    return handlerInput.responseBuilder
      .speak(requestAttributes.t('HELP_REPROMPT'))
      .reprompt(requestAttributes.t('HELP_REPROMPT'))
      .getResponse();
  },
};

const YesIntent = {
  canHandle(handlerInput) {
    // only start a new game if yes is said when not playing a game.
    let isCurrentlyPlaying = false;
    const { attributesManager } = handlerInput;
    const sessionAttributes = attributesManager.getSessionAttributes();

    if (sessionAttributes.gameState &&
      sessionAttributes.gameState === 'STARTED') {
      isCurrentlyPlaying = true;
    }

    return !isCurrentlyPlaying 
      && Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest' 
      && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.YesIntent';
  },
  handle(handlerInput) {
    const { attributesManager } = handlerInput;
    const requestAttributes = attributesManager.getRequestAttributes();
    const sessionAttributes = attributesManager.getSessionAttributes();
    
    sessionAttributes.gameState = 'STARTED';
    
    var answers = new Array(4);
    
    for(var j = 0; j<4; j++) {
        var randomColorIndex = Math.floor(Math.random() * 4);
        answers[j] = colors[randomColorIndex];
    }
    
    sessionAttributes.answerColor = answers;
    
    return handlerInput.responseBuilder
      .speak(requestAttributes.t('YES_MESSAGE', sessionAttributes.answerColor))
      .reprompt(requestAttributes.t('HELP_MESSAGE'))
      .getResponse();
  },
};


const NoIntent = {
  canHandle(handlerInput) {
    // only treat no as an exit when outside a game
    let isCurrentlyPlaying = false;
    const { attributesManager } = handlerInput;
    const sessionAttributes = attributesManager.getSessionAttributes();

    if (sessionAttributes.gameState &&
      sessionAttributes.gameState === 'STARTED') {
      isCurrentlyPlaying = true;
    }

    return !isCurrentlyPlaying 
      && Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest' 
      && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.NoIntent';
  },
  async handle(handlerInput) {
    const { attributesManager } = handlerInput;
    const requestAttributes = attributesManager.getRequestAttributes();
    const sessionAttributes = attributesManager.getSessionAttributes();

    sessionAttributes.endedSessionCount += 1;
    sessionAttributes.gameState = 'ENDED';
    attributesManager.setPersistentAttributes(sessionAttributes);

    await attributesManager.savePersistentAttributes();

    return handlerInput.responseBuilder
      .speak(requestAttributes.t('EXIT_MESSAGE'))
      .getResponse();

  },
};

const UnhandledIntent = {
  canHandle() {
    return true;
  },
  handle(handlerInput) {
    const requestAttributes = handlerInput.attributesManager.getRequestAttributes();

    return handlerInput.responseBuilder
      .speak(requestAttributes.t('CONTINUE_MESSAGE'))
      .reprompt(requestAttributes.t('CONTINUE_MESSAGE'))
      .getResponse();
  },
};

const ColorGuessIntent = {
  canHandle(handlerInput) {
    
    return Alexa.getIntentName(handlerInput.requestEnvelope) === 'ColorGuessIntent';
  },
  async handle(handlerInput) {
    
    const { attributesManager } = handlerInput;
    const requestAttributes = attributesManager.getRequestAttributes();
    const sessionAttributes = attributesManager.getSessionAttributes();
    
    var guess = new Array(4);
    
    
    for(var i = 0; i<4; i++ ) {
        var guessColor = Alexa.getSlotValue(handlerInput.requestEnvelope, 'color');    
        guess = guessColor.split(" ");
    }
    sessionAttributes.guessColor = guess;
    
    if (sessionAttributes.guessColor[0] !== sessionAttributes.answerColor[0] || sessionAttributes.guessColor[1] !== sessionAttributes.answerColor[1] || sessionAttributes.guessColor[2] !== sessionAttributes.answerColor[2]
    || sessionAttributes.guessColor[3] !== sessionAttributes.answerColor[3]) {
        var temp = score;
        attributesManager.savePersistentAttributes();
        return handlerInput.responseBuilder
            .speak(requestAttributes.t('GUESS_WRONG_MESSAGE', sessionAttributes.answerColor, sessionAttributes.guessColor, temp))
            .repromt("Say the correct answer")
            .getResponse();
    }
    else if(sessionAttributes.guessColor[0] === sessionAttributes.answerColor[0] && sessionAttributes.guessColor[1] === sessionAttributes.answerColor[1] && sessionAttributes.guessColor[2] === sessionAttributes.answerColor[2] 
    && sessionAttributes.guessColor[3] === sessionAttributes.answerColor[3]){
        score += 1;
         sessionAttributes.gameState = 'ENDED';
         attributesManager.setPersistentAttributes(sessionAttributes);
         await attributesManager.savePersistentAttributes();
         return handlerInput.responseBuilder
            .speak(requestAttributes.t('GUESS_CORRECT_MESSAGE', sessionAttributes.guessColor, score))
            .reprompt(requestAttributes.t('CONTINUE_MESSAGE'))
            .getResponse();
    }
    return handlerInput.responseBuilder
      .speak(requestAttributes.t('FALLBACK_MESSAGE_DURING_GAME'))
      .reprompt(requestAttributes.t('FALLBACK_REPROMPT_DURING_GAME'))
      .getResponse();
  },
};

const ErrorHandler = {
  canHandle() {
    return true;
  },
  handle(handlerInput, error) {
    console.log(`Error handled: ${error.message}`);
    console.log(`Error stack: ${error.stack}`);
    const requestAttributes = handlerInput.attributesManager.getRequestAttributes();
    const sessionAttributes = handlerInput.attributesManager.getSessionAttributes();
    
    return handlerInput.responseBuilder
      .speak(requestAttributes.t('GUESS_WRONG_MESSAGE', sessionAttributes.answerColor, sessionAttributes.guessColor, score))
      .reprompt(requestAttributes.t('ERROR_MESSAGE'))
      .getResponse();
  },
};

const FallbackHandler = {
  canHandle(handlerInput) {
    // handle fallback intent, yes and no when playing a game
    // for yes and no, will only get here if and not caught by the normal intent handler
    return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
      && (Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.FallbackIntent' 
      || Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.YesIntent'
      || Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.NoIntent');
  },
  handle(handlerInput) {
    const { attributesManager } = handlerInput;
    const requestAttributes = attributesManager.getRequestAttributes();
    const sessionAttributes = attributesManager.getSessionAttributes();

    if (sessionAttributes.gameState && sessionAttributes.gameState === 'STARTED') {
      // currently playing
      return handlerInput.responseBuilder
        .speak(requestAttributes.t('FALLBACK_MESSAGE_DURING_GAME'))
        .reprompt(requestAttributes.t('FALLBACK_REPROMPT_DURING_GAME'))
        .getResponse();
    }

    // not playing
    return handlerInput.responseBuilder
      .speak(requestAttributes.t('FALLBACK_MESSAGE_OUTSIDE_GAME'))
      .reprompt(requestAttributes.t('CONTINUE_MESSAGE'))
      .getResponse();
  },
};

const LocalizationInterceptor = {
  process(handlerInput) {
    const localizationClient = i18n.use(sprintf).init({
      lng: Alexa.getLocale(handlerInput.requestEnvelope),
      resources: languageStrings,
    });
    localizationClient.localize = function localize() {
      const args = arguments;
      const values = [];
      for (let i = 1; i < args.length; i += 1) {
        values.push(args[i]);
      }
      const value = i18n.t(args[0], {
        returnObjects: true,
        postProcess: 'sprintf',
        sprintf: values,
      });
      if (Array.isArray(value)) {
        return value[Math.floor(Math.random() * value.length)];
      }
      return value;
    };
    const attributes = handlerInput.attributesManager.getRequestAttributes();
    attributes.t = function translate(...args) {
      return localizationClient.localize(...args);
    };
  },
};

function getPersistenceAdapter() {
   // Determines persistence adapter to be used based on environment
    const s3Adapter = require('ask-sdk-s3-persistence-adapter');
    return new s3Adapter.S3PersistenceAdapter({
      bucketName: process.env.S3_PERSISTENCE_BUCKET,
    });
}

const skillBuilder = Alexa.SkillBuilders.custom();

exports.handler = skillBuilder
  .withPersistenceAdapter(getPersistenceAdapter())
  .addRequestHandlers(
    LaunchRequest,
    ExitHandler,
    SessionEndedRequest,
    HelpIntent,
    YesIntent,
    NoIntent,
    ColorGuessIntent,
    FallbackHandler,
    UnhandledIntent,
  )
  .addRequestInterceptors(LocalizationInterceptor)
  .addErrorHandlers(ErrorHandler)
  .lambda();

