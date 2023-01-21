import { getPosts } from '$lib/utils/blog';
import type { PageLoad } from './$types';

export const load = (async () => {
	const posts = await getPosts();

	return {
		posts
	};
}) satisfies PageLoad;
