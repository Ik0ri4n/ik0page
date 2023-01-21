<script lang="ts">
	import PostPreview from '$lib/components/PostPreview.svelte';
	import type { PageData } from './$types';

	export let data: PageData;
</script>

<article class="mx-auto max-w-3xl">
	<h1 class="text-2xl font-medium text-gray-900 dark:text-gray-50 sm:text-3xl">Posts</h1>

	<ul class="mt-2 space-y-2 sm:mt-4 sm:space-y-4">
		{#each data.posts as post}
			<li>
				<PostPreview {post} />
			</li>
		{/each}
	</ul>
</article>
