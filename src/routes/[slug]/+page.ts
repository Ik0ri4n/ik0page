import { getPost } from '$lib/utils/blog';
import type { PageLoad } from './$types';

export const load = (async ({ params }) => {
	return getPost(params.slug);
}) satisfies PageLoad;
