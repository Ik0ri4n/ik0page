<script lang="ts">
	import PostPreview from '$lib/components/PostPreview.svelte';
	import type { PageData } from './$types';

	export let data: PageData;
</script>

<article class="mx-auto max-w-3xl">
	<h1 class="text-2xl font-medium text-gray-900 dark:text-gray-50 sm:text-3xl">Welcome to <span class="text-green-700 dark:text-green-400">Ik0</span>blog</h1>

	<p class="clear-both mt-4 sm:mt-6">A journey through CTFs, cybersecurity and the broad field that is computer science.</p>

	<h2 class="mt-4 sm:mt-6 text-xl font-semibold text-gray-900 dark:text-gray-50 sm:text-2xl">Latest Posts</h2>

	<ul class="mt-2 space-y-2 sm:mt-4 sm:space-y-4">
		{#each data.posts as post}
			<li>
				<PostPreview {post} />
			</li>
		{/each}
	</ul>
</article>
