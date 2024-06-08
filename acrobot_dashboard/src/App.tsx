import { Limb, LimbName } from '@/components/limb/Limb';
import { Button } from '@/components/ui/button';
import { GetStatusResponse, acrobot } from '@/lib/AcroborAPI';
import { useState } from 'react';

function App() {
  const [response, setResponse] = useState<Partial<GetStatusResponse>>({});

  return (
    <main className="flex min-w-full flex-col items-center justify-center pt-2">
      <div className="flex flex-col">
        <Button onClick={async () => setResponse(await acrobot.getStatus())}>Get status</Button>
      </div>
      <div className="grid w-full grid-cols-2 grid-rows-2 gap-4 p-2">
        <Limb status={response.rightArm}>
          <LimbName>Right arm</LimbName>
        </Limb>
        <Limb status={response.leftArm}>
          <LimbName>Left arm</LimbName>
        </Limb>
        <Limb status={response.rightLeg}>
          <LimbName>Right leg</LimbName>
        </Limb>
        <Limb status={response.leftLeg}>
          <LimbName>Left leg</LimbName>
        </Limb>
      </div>
    </main>
  );
}

export default App;
