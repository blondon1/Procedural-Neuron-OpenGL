import { useEffect, useState } from "react";

function detectBrowserSupport() {
  const hasWebAssembly = typeof WebAssembly !== "undefined";
  const testCanvas = document.createElement("canvas");
  const hasWebGL2 = Boolean(testCanvas.getContext("webgl2"));

  return hasWebAssembly && hasWebGL2;
}

function UnsupportedBrowserCard() {
  return (
    <main className="grid min-h-screen place-items-center bg-black px-6 text-white">
      <section className="max-w-md rounded-lg border border-[rgba(229,229,234,0.2)] bg-[rgba(0,0,0,0.7)] p-6 text-center shadow-2xl backdrop-blur-md">
        <p className="m-0 text-[17px] leading-relaxed text-[rgba(255,255,255,0.92)]">
          This simulation requires a desktop browser with WebGL2 and WebAssembly
          support. Please open in the latest Chrome, Firefox, or Edge.
        </p>
      </section>
    </main>
  );
}

export default function BrowserGate({ children }) {
  const [isSupported, setIsSupported] = useState(null);

  useEffect(() => {
    setIsSupported(detectBrowserSupport());
  }, []);

  if (isSupported === false) {
    return <UnsupportedBrowserCard />;
  }

  if (isSupported === null) {
    return <main className="min-h-screen bg-black" />;
  }

  return children;
}
