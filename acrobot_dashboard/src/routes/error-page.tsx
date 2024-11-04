import { useRouteError } from 'react-router-dom';

export function ErrorPage() {
  const error = useRouteError();
  console.error(error);
  const statusText = typeof error === 'object' && error !== null && 'statusText' in error && `${error.statusText}`;
  const message = typeof error === 'object' && error !== null && 'message' in error && `${error.message}`;

  return (
    <div className="flex h-full w-full flex-col items-center justify-center">
      <h1>Oops!</h1>
      <p>Sorry, something went wrong.</p>
      <p>
        <i>{statusText || message || 'Unknown'}</i>
      </p>
    </div>
  );
}
