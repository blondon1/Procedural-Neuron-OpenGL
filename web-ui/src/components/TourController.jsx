import { useEffect, useRef, useState } from "react";

const TOUR_STOPS = [
  {
    delay: 0,
    text: "Welcome to In Silico. You are observing a procedurally generated, 1,000-node neural graph.",
  },
  {
    delay: 5000,
    text: "Notice the Signed Distance Field blending on the membranes as the action potential propagates.",
  },
];

function speak(text) {
  if (!("speechSynthesis" in window)) {
    return;
  }

  const utterance = new SpeechSynthesisUtterance(text);
  utterance.rate = 0.92;
  utterance.pitch = 0.96;
  utterance.volume = 0.92;
  window.speechSynthesis.speak(utterance);
}

export default function TourController() {
  const [tourState, setTourState] = useState("idle");
  const [currentStop, setCurrentStop] = useState(-1);
  const timeoutRefs = useRef([]);

  useEffect(() => {
    return () => {
      timeoutRefs.current.forEach((timeoutId) => window.clearTimeout(timeoutId));
      window.speechSynthesis?.cancel();
    };
  }, []);

  function startTour() {
    timeoutRefs.current.forEach((timeoutId) => window.clearTimeout(timeoutId));
    timeoutRefs.current = [];
    window.speechSynthesis?.cancel();

    setTourState("running");
    setCurrentStop(-1);

    TOUR_STOPS.forEach((stop, index) => {
      const timeoutId = window.setTimeout(() => {
        setCurrentStop(index);
        speak(stop.text);

        if (index === TOUR_STOPS.length - 1) {
          setTourState("complete");
        }
      }, stop.delay);

      timeoutRefs.current.push(timeoutId);
    });
  }

  const buttonLabel =
    tourState === "running" ? "Tour in Progress" : "Start Guided Tour";

  return (
    <section className="absolute right-6 top-6 pointer-events-auto w-[min(300px,calc(100vw-48px))] rounded-lg border border-[rgba(229,229,234,0.2)] bg-[rgba(0,0,0,0.7)] p-4 text-white shadow-2xl backdrop-blur-md">
      <p className="mb-3 text-xs font-medium text-[rgba(235,235,245,0.58)]">
        Cinematic tour
      </p>
      <button
        className="w-full rounded-md border border-[rgba(229,229,234,0.22)] bg-[rgba(255,255,255,0.1)] px-4 py-3 text-sm font-semibold text-white transition hover:bg-[rgba(255,255,255,0.16)] focus:outline-none focus:ring-2 focus:ring-white/70"
        type="button"
        disabled={tourState === "running"}
        onClick={startTour}
      >
        {buttonLabel}
      </button>
      <p className="mt-3 min-h-10 text-sm leading-snug text-[rgba(235,235,245,0.68)]">
        {currentStop >= 0 ? TOUR_STOPS[currentStop].text : "Ready for narration."}
      </p>
    </section>
  );
}
