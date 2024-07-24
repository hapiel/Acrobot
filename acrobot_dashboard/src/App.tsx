import { Limb, LimbName } from '@/components/limb/Limb';
import { useRobotStatus } from '@/hooks/useRobotState';
import { useWsStatus } from '@/hooks/useWsStatus';
import useWebSocket, { ReadyState } from 'react-use-websocket';

function App() {
  const {
    status: { rightArm, rightLeg, leftArm, leftLeg },
    setStatus
  } = useRobotStatus();

  const wsStatus = useWsStatus();

  const { readyState } = useWebSocket('ws://acrobot.local:3000/ws', {
    shouldReconnect: () => true,
    onMessage: (message) => setStatus(JSON.parse(message.data)),
    onOpen: () => wsStatus.set(ReadyState.OPEN),
    onClose: () => wsStatus.set(ReadyState.CLOSED),
    onError: () => wsStatus.set(ReadyState.CLOSED)
  });

  return (
    <main className="flex min-w-full flex-col items-center justify-center pt-2">
      <div className="flex flex-col gap-2"></div>
      <div>
        <h2>Socket status</h2>
        <p>{ReadyState[readyState]}</p>
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
