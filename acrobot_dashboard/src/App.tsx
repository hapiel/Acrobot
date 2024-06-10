import { Limb, LimbName } from '@/components/limb/Limb';
import { Button } from '@/components/ui/button';
import { useRobotStatus } from '@/hooks/useRobotState';
import { acrobot } from '@/lib/AcroborAPI';

function App() {
  const {
    status: { rightArm, rightLeg, leftArm, leftLeg },
    setStatus
  } = useRobotStatus();

  return (
    <main className="flex min-w-full flex-col items-center justify-center pt-2">
      <div className="flex flex-col">
        <Button onClick={async () => setStatus(await acrobot.getStatus())}>Get status</Button>
      </div>
      <div className="grid w-full grid-cols-2 grid-rows-2 gap-4 p-2">
        <Limb status={rightArm}>
          <LimbName>Right arm</LimbName>
        </Limb>
        <Limb status={leftArm}>
          <LimbName>Left arm</LimbName>
        </Limb>
        <Limb status={rightLeg}>
          <LimbName>Right leg</LimbName>
        </Limb>
        <Limb status={leftLeg}>
          <LimbName>Left leg</LimbName>
        </Limb>
      </div>
    </main>
  );
}

export default App;
