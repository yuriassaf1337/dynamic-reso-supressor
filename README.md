<img width="1200" height="406" alt="UI" src="https://github.com/user-attachments/assets/5ac336a7-a177-410f-bc4d-219507a460a0" />

## DyERS: Dynamic Equalizer / Resonance Suppressor

DyERS is a dependency-free spectral-based Dynamic Equalizer/Resonance Suppressor. The main idea of the algorithm can be described as: 

(On an WOLA processing)
1. Find prominent spectral peaks
2. Generate Gaussian-like "notch filter" gains
3. Apply gains directly on the spectrum without modifying the phase

This project was inspired on the plugin Soothe2 (https://oeksound.com/plugins/soothe2/), from which I saw it once, found it really cool(!), and wanted to implement it for fun. The functionality here is considerably reduced compared to Soothe, but the implementation and functionality should be enough for many use cases. Additionally, it can be a really good start for people who want to implement an audio plugin with some spectral-based processing. 

You can check the plugin in action on the video below: 



https://github.com/user-attachments/assets/4a45c2c5-5ab4-46c6-9717-2235c931fcc6



## Installation 

You can open the Projucer project and run like a regular JUCE plugin. There's no external dependencies besides JUCE, thus it should be trivial to make it work. 

For those who may just want to test the plugin and don't necessarily want to compile it by themselves, I've also added (Silicon) MacOS VST-AU binaries as releases if you want to download it. MacOS will flag the files since they're not registered, one possible solution can be found here https://www.motunation.com/forum/viewtopic.php?t=72872. It involves removing the files from quarantine: 

```
xattr -rd com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/DynamicEqualizer.vst3
xattr -rd com.apple.quarantine ~/Library/Audio/Plug-Ins/Components/DynamicEqualizer.component

```
optionally also signing them: 

```if it doesn't work, you can try to sign them your
codesign --deep --force --sign - ~/Library/Audio/Plug-Ins/VST3/DynamicEqualizer.vst3
codesign --deep --force --sign - ~/Library/Audio/Plug-Ins/Components/DynamicEqualizer.component
```

### Controls 

Some short description about the controls and functionality: 

* **Resonance Gain**: Basically how much you want to decrease the resonances (or increase, sometimes it sounds kind of cool).
* **Sensitivity**: How easily the equalizer will react to a peak. The lower this value, the stronger the peak must be to be recognized.
* **Sharpness**: Approximately the Q-value of each pseudo-notch filter.  
* **Speed**: How fast the method will adapt. Is basically a single knob for attack/release.
* **MakeUp**: Simple gain, necessary since you're removing energy from the signal but likely wants mainly to remove the strength of the resonances.  
* **Wet**: How much of the processed signal should be used (You'll likely never need less than full). 
* **Difference**: Basically a "inverse delta". It lets you hear clearly what you've removed.
* **Passthrough**: Skip plugin processing (but keep spectral chain). Used to debug OLA/WOLA if you want to try new things. 
* **FFTSize**: This controls the trade-off between frequency resolution and latency. Higher values can better resolve peaks, but are more computationally expensive and have higher latency. For very fast and sharp changes, you may need longer FFT sizes to avoid small artefacts (or programatically increase the overlap ratio). 

Additionally, you can change the frequency range of the processing by left/right mouse click on the spectrum. 

## Disclaimer 

I did this project a while ago mostly for fun and decided to do a general clean-up to post it, however, some edge-cases may still not have been thoroughly considered. I'm sharing mostly because I believe some people may be curious to see how a spectral-based dynamic equalizer/resonance supressor actually work on code (at least one of the many possible implementations of it!). 

